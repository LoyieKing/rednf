# -*- coding: utf-8 -*-
"""保底挂单（engine.floor）：每个建模道具（材料 + 装备）始终保持至少 min_visible 张可见系统挂单，没有上限。

设计（见 README「保底挂单」）：
- 纯维护：floor 只补「缺口」，不改变材料经济模型的活跃度——floor 挂单（note 以 floor 开头）对材料引擎完全不可见
  （不进挂单簿、不占 target_listings、到期不触发引擎补挂），引擎照旧挂/买它自己的货。
- 覆盖数 = 该道具 active、非手动的系统挂单里，距离 7200 秒到期还超过 lead_minutes 的张数（引擎/shelf/floor 挂单都算）。
  覆盖数 < min_visible 就补挂。提前 lead_minutes 补挂 → 旧单还没下架新单已经挂上，不会出现可见空档。
- 平滑：令牌桶限速 rate = min(max_new_per_min, 1.15 × 目标总张数 / ramp_minutes)；每轮先补覆盖数最少的道具，
  所以首次铺货时每个道具的 10 张分散在 ramp_minutes 里挂出，之后到期/补挂时间自然保持均匀（≈ 总张数 / (120-lead) 张/分钟）。
- 拍卖服硬上限：系统挂单到期处理每 2 秒最多 3 张（ORIG Auction::ProcessMostRecentExpireItem cnt2<=2，定时器 2000ms），
  即 90 张/分钟，与引擎的系统挂单共用。所以 max_new_per_min 默认 60（全量稳态约 53 张/分钟）。
- 虚拟买家：装备 floor 挂单（以及 floor 覆盖层级里的 shelf 挂单）用极低的风险率 h = -ln(1-target_sell_through)/((120-min_shelf)/60)，
  同样要挂满 min_shelf_minutes 才会买；与装备共用 virtual_fill.max_per_day（300/天）。材料 floor 挂单默认不被虚拟买家买（活跃度不变），
  玩家可以正常购买。
- 价格：装备 = item.ref × 层级公允价 × exp(N(0.02,0.05)) / 层级 ref（夹在单件价格带）；材料 = 公允价 × exp(N(0.03,0.04))（夹在价格带），
  数量按模型 lot_qty 抽。假紫每张都独立按服务器规则生成随机属性。
"""
import math
import random
import time

from .engine import lot, round_price

NOTE_STACK = "floor"
NOTE_EQ = "floor:eq"


def is_floor(note):
    return str(note or "").startswith("floor")


class FloorRunner:
    def __init__(self, runner):
        self.runner = runner
        self.bot = runner.bot
        self.rng = random.Random()
        self.tokens = 0.0
        self.last_tick = None
        self.last = {}
        self.minute = []          # 最近 60 秒的上架时间戳

    @property
    def cfg(self):
        return (self.runner.cfg.get("floor") or {})

    @property
    def enabled(self):
        return bool(self.cfg.get("enabled", False))

    # ---------------- 范围 ----------------
    def scope(self):
        """{iid: "stack"|"eq"}：floor 覆盖的道具。scope.all=true = 全部建模道具；否则 scope.tiers（装备层级）+ scope.items。"""
        sc = self.cfg.get("scope") or {}
        eq = self.runner.equip
        out = {}
        tiers = {str(t) if str(t).startswith("EQ_") else "EQ_" + str(t) for t in (sc.get("tiers") or [])}
        items = {int(x) for x in (sc.get("items") or [])}
        for iid in self.runner.items:
            if sc.get("all") or iid in items:
                if iid not in self.runner.excluded and iid not in self.bot.disabled_items:
                    out[iid] = "stack"
        if eq.listing_enabled:
            for iid, ci in eq.catalog.items():
                if (sc.get("all") or ci["tier"] in tiers or iid in items) and iid not in self.bot.disabled_items:
                    out[iid] = "eq"
        return out

    def covers_tier(self, tier):
        """装备层级是否整体由 floor 维护（是的话 shelf 不再补挂/虚拟购买，交给 floor）。"""
        if not self.enabled:
            return False
        sc = self.cfg.get("scope") or {}
        if sc.get("all"):
            return True
        tiers = {str(t) if str(t).startswith("EQ_") else "EQ_" + str(t) for t in (sc.get("tiers") or [])}
        return tier in tiers

    @property
    def min_visible(self):
        return max(0, int(self.cfg.get("min_visible", 10)))

    def eq_hazard(self, tier):
        """装备 floor 的风险率（每小时）：按 target_sell_through 在 (7200 秒 - min_shelf) 内卖出的比例反推。
        （-114514 挂单在架 12~36 小时，但风险率按小时计、在架张数不变，所以每天的虚拟成交笔数与 2 小时挂单时相同。）"""
        ec = self.cfg.get("equipment") or {}
        if ec.get("buy_hazard_per_h") is not None:
            return float(ec["buy_hazard_per_h"])
        s = min(max(float(ec.get("target_sell_through", 0.004)), 0.0), 0.99)
        sh = self.runner.equip.shelf(tier) or {}
        ms = float(ec.get("min_shelf_minutes", sh.get("min_shelf_minutes", 30)))
        return -math.log(1 - s) / max((120.0 - ms) / 60.0, 1e-6)

    # ---------------- 覆盖统计 ----------------
    def coverage(self, now=None):
        """{iid: [row...]}：floor 范围内每个道具的 active、非手动系统挂单（含引擎/shelf/floor）。"""
        now = now or time.time()
        out = {}
        for r in self.bot.store.query(
                "SELECT auction_id, item_id, count_now, unit_price, instant_price, listed_at, expire_at, note FROM bot_listings "
                "WHERE status='active' AND count_now>0 AND COALESCE(note,'') NOT LIKE 'manual%'"):
            if r["expire_at"] - 90 <= now:
                continue
            out.setdefault(r["item_id"], []).append(r)
        return out

    def status(self):
        sc = self.scope() if self.enabled else {}
        cov = self.coverage()
        lead = float(self.cfg.get("lead_minutes", 10)) * 60
        now = time.time()
        n_vis = sum(len(cov.get(i, [])) for i in sc)
        n_cov = sum(min(self.min_visible, sum(1 for r in cov.get(i, []) if r["expire_at"] - lead > now)) for i in sc)
        short = sum(1 for i in sc if len(cov.get(i, [])) < self.min_visible)
        fl = self.bot.store.query("SELECT COUNT(*) n FROM bot_listings WHERE status='active' AND note LIKE 'floor%'")[0]["n"]
        return {"enabled": self.enabled, "min_visible": self.min_visible, "items": len(sc),
                "items_eq": sum(1 for v in sc.values() if v == "eq"), "items_stack": sum(1 for v in sc.values() if v == "stack"),
                "target": len(sc) * self.min_visible, "visible": n_vis, "covered": n_cov, "items_below_min": short,
                "floor_listings": fl, "rate_per_min": round(self.rate(len(sc)), 1), "last": self.last,
                "scope": self.cfg.get("scope") or {}}

    def rate(self, n_items):
        c = self.cfg
        total = n_items * self.min_visible
        ramp = max(float(c.get("ramp_minutes", 110)), 0.001)
        return min(float(c.get("max_new_per_min", 60)), max(1.0, 1.15 * total / ramp))

    # ---------------- 一轮 ----------------
    async def tick(self, now=None):
        b = self.bot
        res = {"ts": time.time(), "listed": 0, "eq_listed": 0, "stack_listed": 0, "sold": 0, "errors": 0, "need": 0,
               "items_below_min": 0}
        if not self.enabled or self.runner.engine is None:
            self.last_tick = None
            return res
        c = self.cfg
        now = now or time.time()
        dt = (now - self.last_tick) if self.last_tick else float(c.get("interval_sec", 20))
        self.last_tick = now
        sc = self.scope()
        rate = self.rate(len(sc))
        self.tokens = min(self.tokens + rate * dt / 60.0, max(rate, 1.0))
        lead = float(c.get("lead_minutes", 10)) * 60
        cov = self.coverage(now)
        mv = self.min_visible
        deficits = []
        for iid, kind in sc.items():
            rows = cov.get(iid, [])
            covered = sum(1 for r in rows if r["expire_at"] - lead > now)
            if len(rows) < mv:
                res["items_below_min"] += 1
            if covered < mv:
                deficits.append((covered, self.rng.random(), iid, kind, mv - covered))
        deficits.sort()
        res["need"] = sum(d[4] for d in deficits)
        # 每轮按覆盖数从少到多，一个道具一次补一张，轮转直到令牌用完
        self.minute = [x for x in self.minute if now - x < 60]
        cap_min = int(c.get("max_new_per_min", 60))
        queue = [list(d) for d in deficits]
        stop = False
        while queue and not stop:
            nxt = []
            for d in queue:
                if self.tokens < 1 or len(self.minute) >= cap_min:
                    stop = True
                    break
                ok = await self._list(d[2], d[3], res)
                if ok is None:            # 拍卖服拒绝（繁忙/服务未就绪）：本轮停止
                    stop = True
                    break
                self.tokens -= 1
                if ok:
                    self.minute.append(time.time())
                    d[4] -= 1
                    if d[4] > 0:
                        nxt.append(d)
            queue = nxt
        await self._virtual_buy(now, dt, sc, cov, res)
        self.last = res
        if res["listed"] or res["sold"] or res["errors"]:
            b.log("engine", "保底挂单：补挂 %d（装备 %d、材料 %d），虚拟成交 %d，错误 %d；缺口 %d 张，%d 个道具低于 %d 张"
                  % (res["listed"], res["eq_listed"], res["stack_listed"], res["sold"], res["errors"], res["need"],
                     res["items_below_min"], mv), level="debug")
        return res

    async def _list(self, iid, kind, res):
        """补挂一张。返回 True 成功 / False 失败（跳过这个道具）/ None 停止本轮。"""
        b = self.bot
        if kind == "eq":
            eq = self.runner.equip
            if not await eq.ensure_ok(iid):
                return False
            t = eq.catalog[iid]["tier"]
            p = eq.tiers[t]
            fair = float(eq.engine.fair(t)[0])
            px_t = fair * math.exp(self.rng.gauss(0.02, 0.05))
            px_t = round_price(max(px_t, float(p.get("c", 0)) * float(p.get("price_mult", 1.0)) * 0.9))
            px = eq.item_price(iid, px_t)
            r, f = await eq.list_one(iid, px, NOTE_EQ)
            what = "装备 %s（%s）一口价 %d" % (b.item_name(iid), t, px)
        else:
            if not await b.ensure_item_ok(iid):
                return False
            p = self.runner.items[iid]
            fair = float(self.runner.engine.fair(iid)[0])
            lo, hi = self.runner.band(iid)
            unit = int(min(max(round_price(fair * math.exp(self.rng.gauss(0.03, 0.04))), lo), hi))
            qty = max(1, int(lot(p["lot_qty"], self.rng)))
            lim = int(b.item_meta(iid).get("stack_limit") or 0)
            if lim > 0:
                qty = min(qty, lim)
            r = await b.client.regist_item(b.seller_id, "", iid, qty, unit)
            if r["ok"] and r["auction_id"]:
                b.record_listing(r["auction_id"], iid, qty, unit, unit * qty, NOTE_STACK)
            what = "%s ×%d 单价 %d" % (b.item_name(iid), qty, unit)
        if not r["ok"] or not r["auction_id"]:
            res["errors"] += 1
            b.log("error", "保底挂单失败 %s：%s" % (what, r.get("because_text") or "拍卖号为 0"), level="warn")
            return None if r.get("because") in (0x9e, 0x31) else False
        res["listed"] += 1
        res["eq_listed" if kind == "eq" else "stack_listed"] += 1
        b.log("list", "保底挂单 %s（拍卖号 %d）" % (what, r["auction_id"]), level="debug", auction_id=r["auction_id"], item_id=iid)
        return True

    async def _virtual_buy(self, now, dt, sc, cov, res):
        """极低风险率的虚拟买家：装备 floor 范围内挂满 min_shelf 的系统挂单；材料 floor 挂单只有 stackable.buy_hazard_per_h>0 才买。"""
        b = self.bot
        c = self.cfg
        eq = self.runner.equip
        dt_h = min(max(dt / 3600.0, 1 / 3600.0), 0.5)
        plan = eq.engine and self.runner.engine
        if not plan:
            return
        from .engine import online_now
        k_on = online_now(self.runner.model["global"], now) / (eq.online_mean or 800.0)
        vc = eq.cfg.get("virtual_fill") or {}
        day_cap = int(vc.get("max_per_day", 60))
        t0 = self.runner.today_counts()[2]
        vf_today = b.store.query("SELECT COUNT(*) n FROM deals WHERE ts>=? AND side='virtual' AND source='engine-eq'", (t0,))[0]["n"]
        sh_stack = float((c.get("stackable") or {}).get("buy_hazard_per_h", 0) or 0)
        ec = c.get("equipment") or {}
        for iid, kind in sc.items():
            for r in cov.get(iid, []):
                if kind == "eq":
                    t = eq.catalog[iid]["tier"]
                    if not (is_floor(r["note"]) or (str(r["note"] or "").startswith("eq:") and self.covers_tier(t))):
                        continue
                    sh = eq.shelf(t) or {}
                    ms = float(ec.get("min_shelf_minutes", sh.get("min_shelf_minutes", 30))) * 60
                    if now - r["listed_at"] < ms:
                        continue
                    p = eq.tiers[t]
                    fair = float(eq.engine.fair(t)[0])
                    norm = r["instant_price"] * float(p["ref"]) / eq.catalog[iid]["ref"]
                    sens = float(sh.get("price_sensitivity", 5.0 * float(p.get("epsilon", 1.5))))
                    prob = 1 - math.exp(-self.eq_hazard(t) * math.exp(-sens * (norm / fair - 1)) * k_on * dt_h)
                    if self.rng.random() >= prob:
                        continue
                    if vf_today >= day_cap:
                        return
                    rr = await b.client.buy_now(b.vbuyer_id, b.vbuyer_name, r["auction_id"], r["instant_price"])
                    src, qty, unit, total = "engine-eq", 1, r["instant_price"], r["instant_price"]
                else:
                    if sh_stack <= 0 or not is_floor(r["note"]):
                        continue
                    if now - r["listed_at"] < float((c.get("stackable") or {}).get("min_shelf_minutes", 30)) * 60:
                        continue
                    if self.rng.random() >= 1 - math.exp(-sh_stack * k_on * dt_h):
                        continue
                    qty, unit = r["count_now"], r["unit_price"]
                    rr = await b.client.buy_apiece(b.vbuyer_id, r["auction_id"], unit, qty)
                    src, total = "floor", unit * qty
                if rr.get("ok"):
                    if src == "engine-eq":
                        vf_today += 1
                    res["sold"] += 1
                    b.store.execute("UPDATE bot_listings SET count_now=0, status='sold', closed_at=? WHERE auction_id=?",
                                    (time.time(), r["auction_id"]))
                    b.store.add_deal(time.time(), iid, r["auction_id"], qty, unit, total, "virtual", b.seller_id, b.vbuyer_id, src,
                                     "fl-v:%d:%d" % (r["auction_id"], int(time.time() * 1000)))
                    b.log("deal", "保底挂单虚拟成交 %s ×%d 单价 %d（拍卖号 %d，挂了 %.0f 分钟）"
                          % (b.item_name(iid), qty, unit, r["auction_id"], (now - r["listed_at"]) / 60), auction_id=r["auction_id"])
                else:
                    res["errors"] += 1
                    b.log("error", "保底挂单虚拟成交失败 拍卖号 %d：%s" % (r["auction_id"], rr.get("because_text")), level="warn")
