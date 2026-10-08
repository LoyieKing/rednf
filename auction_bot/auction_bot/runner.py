# -*- coding: utf-8 -*-
"""把经济模型引擎（engine.py）接到真实拍卖服：取挂单簿 → 模型一步 → 按护栏执行上架/成交。"""
import json
import math
import os
import random
import time
import traceback

from .engine import Engine

FAKE_BUYER = 1999999999


def load_json(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def load_excluded(path):
    """excluded_items.json → {item_id: 原因}。晶块类（NPC 可兑换）+ 无价格锚点 两类都排除。"""
    out = {}
    if not path or not os.path.exists(path):
        return out
    d = load_json(path)
    for e in d.get("excluded_npc_shop_convertible", []):
        out[int(e["id"])] = "NPC 可兑换（%s）" % e.get("name", "")
    for e in d.get("not_modelled_no_price_anchor", []):
        out[int(e["id"])] = "无价格锚点（%s）" % e.get("name", "")
    return out


class EngineRunner:
    def __init__(self, bot):
        self.bot = bot
        self.engine = None
        self.model = None
        self.items = {}
        self.skipped = {}            # iid -> 原因
        self.next_step_at = 0
        self.last_step_at = None
        self.last_plan = None
        self.expired_pending = {}    # iid -> [剩余数量]
        self.hourly_new = []
        self.reload()

    @property
    def cfg(self):
        return self.bot.cfg.get("engine", {}) or {}

    def _path(self, p):
        if p and not os.path.isabs(p):
            p = os.path.join(self.bot.base_dir, p)
        return p

    def reload(self):
        b = self.bot
        c = self.cfg
        self.excluded = load_excluded(self._path(c.get("excluded_items", "ref/excluded_items.json")))
        for x in c.get("excluded_extra", []) or []:
            self.excluded[int(x)] = "配置中额外排除"
        b.excluded = self.excluded
        for iid in list(b.seller_items):
            if iid in self.excluded:
                del b.seller_items[iid]
        if not c.get("enabled"):
            self.engine = None
            self.items = {}
            return
        mp = self._path(c.get("model_config", "ref/model_config.json"))
        self.model = load_json(mp)
        wl = set(int(x) for x in (c.get("items_whitelist") or []))
        ov = {int(k): v for k, v in (c.get("item_overrides") or {}).items()}
        items, skipped = {}, {}
        for k, p in self.model["items"].items():
            iid = int(k)
            if iid in self.excluded:
                skipped[iid] = "排除名单：" + self.excluded[iid]
                continue
            if wl and iid not in wl:
                skipped[iid] = "不在本轮白名单"
                continue
            o = ov.get(iid) or {}
            if o.get("enabled") is False:
                skipped[iid] = "配置 item_overrides 关闭"
                continue
            meta = b.item_meta(iid)
            if meta and meta.get("kind") != "stackable":
                skipped[iid] = "PVF 中不是可堆叠物品，暂不支持"
                continue
            p = dict(p)
            for key in ("price_mult", "faucet_per_h", "sink_rate_per_h", "activity", "target_listings"):
                if key in o:
                    p[key] = o[key]
            items[iid] = p
        self.items, self.skipped = items, skipped
        state = b.store.kv_get("engine_state") or {}
        self.engine = Engine(self.model, items, c, state=state, rng=random.Random())
        b.watch_ids |= set(items)
        b.log("engine", "经济模型已加载：%d 个道具参与（共 %d 个建模道具，跳过 %d 个），总量系数 volume_scale=%s"
              % (len(items), len(self.model["items"]), len(skipped), c.get("volume_scale", 1.0)))

    def band(self, iid):
        p = self.items[iid]
        lo, hi = p.get("band") or (p["ref"] * 0.7, p["ref"] * 1.3)
        s = float(self.cfg.get("price_band_slack", 0.25))
        return lo * (1 - s), hi * (1 + s)

    def today_counts(self):
        t0 = time.mktime(time.localtime()[:3] + (0, 0, 0, 0, 0, -1))
        r = {x["side"]: x for x in self.bot.store.query(
            "SELECT side, COUNT(*) n, SUM(count) q, SUM(total) g FROM deals WHERE ts>=? GROUP BY side", (t0,))}
        nl = self.bot.store.query("SELECT COUNT(*) n FROM bot_listings WHERE listed_at>=?", (t0,))[0]["n"]
        return r, nl, t0

    def due(self):
        return self.engine is not None and time.time() >= self.next_step_at

    def schedule_next(self):
        lo, hi = self.cfg.get("step_minutes", [5, 15])
        self.next_step_at = time.time() + random.uniform(float(lo), float(hi)) * 60

    async def step(self):
        b = self.bot
        c = self.cfg
        now = time.time()
        self.schedule_next()
        await b.scan_market()               # 刷新挂单簿 + 对账（到期 → expired_pending）
        npc = b.store.kv_get("npc_name")
        mine = {r["auction_id"]: r for r in b.store.query("SELECT auction_id, listed_at FROM bot_listings WHERE status='active'")}
        book = {}
        for iid in self.items:
            asks = []
            for l in b.snapshot.get(iid, []):
                aid = l["auction_id"]
                cnt = l["item"]["add_info"]
                is_sys = aid in mine or b.is_npc_name(l["owner_name"])
                apiece = l["price"] == -1 and l["unit_price"] > 0 and cnt > 0
                asks.append({"auction_id": aid, "price": l["unit_price"] or (l["instant_price"] // max(1, cnt)),
                             "qty": cnt, "owner_id": -1 if is_sys else 0, "seller": "sys" if is_sys else "user",
                             "buyable": apiece and (aid in mine if is_sys else True), "owner_name": l["owner_name"]})
            book[iid] = asks
        dt_h = (now - self.last_step_at) / 3600.0 if self.last_step_at else 10 / 60.0
        expired, self.expired_pending = self.expired_pending, {}
        plan = self.engine.step(dt_h, book, expired, now)
        self.last_step_at = now
        res = {"new_planned": len(plan["new"]), "new_done": 0, "vf_planned": 0, "vf_done": 0, "uf_planned": 0,
               "uf_done": 0, "user_gold": 0, "errors": 0, "skipped": []}
        # ---- 1) 新挂单（限速） ----
        self.hourly_new = [t for t in self.hourly_new if now - t < 3600]
        per_step = int(c.get("max_new_listings_per_step", 40))
        per_hour = int(c.get("max_new_listings_per_hour", 200))
        new_ids = {}
        for idx, n in enumerate(plan["new"]):
            iid = n["item_id"]
            if res["new_done"] >= per_step or len(self.hourly_new) >= per_hour:
                res["skipped"].append("上架限速")
                break
            if iid in self.excluded or not await b.ensure_item_ok(iid):
                continue
            lo, hi = self.band(iid)
            unit = int(min(max(n["price"], lo), hi))
            qty = max(1, int(n["qty"]))
            lim = int(b.item_meta(iid).get("stack_limit") or 0)
            if lim > 0:
                qty = min(qty, lim)
            r = await b.client.regist_item(-1, "", iid, qty, unit)
            if not r["ok"] or not r["auction_id"]:
                res["errors"] += 1
                b.log("error", "模型上架失败 %s ×%d @%d：%s" % (b.item_name(iid), qty, unit, r.get("because_text") or "拍卖号为 0"),
                      level="warn")
                if r.get("because") in (0x9e, 0x31):
                    break
                continue
            aid = r["auction_id"]
            t = time.time()
            b.store.execute("INSERT OR REPLACE INTO bot_listings(auction_id,item_id,count0,count_now,unit_price,instant_price,"
                            "listed_at,expire_at,status,last_seen,note) VALUES(?,?,?,?,?,?,?,?,'active',?,?)",
                            (aid, iid, qty, qty, unit, unit * qty, t, t + 7200, t, n["seller"]))
            new_ids[idx] = (aid, qty, unit)
            self.hourly_new.append(t)
            res["new_done"] += 1
            b.log("list", "模型上架[%s] %s ×%d，单价 %d（拍卖号 %d）" % (
                {"producer": "生产者", "dumper": "抛售者", "flipper": "倒爷", "relist": "到期补挂", "secret_shop_resale": "神秘商店转卖"}.get(n["seller"], n["seller"]),
                b.item_name(iid), qty, unit, aid), auction_id=aid, item_id=iid)
        # ---- 2) 成交：先玩家挂单（真实购买），再系统挂单（假买家） ----
        counts, _, _ = self.today_counts()
        uc = c.get("user_fill", {}) or {}
        vc = c.get("virtual_fill", {}) or {}
        user_gold_today = (counts.get("bot_buy_human") or {}).get("g") or 0
        vf_today = (counts.get("virtual") or {}).get("n") or 0
        vf_step_cap = int(vc.get("max_per_step", 60))
        vf_day_cap = int(vc.get("max_per_day", 5000))
        fills = sorted(plan["fills"], key=lambda f: (not f["user"],))
        for f in fills:
            iid = f["item_id"]
            if iid in self.excluded:
                continue
            if f["user"]:
                res["uf_planned"] += 1
                aid, unit = f["auction_id"], f["unit_price"]
                qty = int(f["qty"])
                cost = unit * qty
                lo, hi = self.band(iid)
                why = None
                budget = int(uc.get("max_gold_per_day", 2000000)) - user_gold_today
                qty = min(qty, int(uc.get("max_gold_per_buy", uc.get("max_gold_per_day", 2000000))) // max(1, unit), budget // max(1, unit))
                cost = unit * qty
                if not uc.get("enabled", False):
                    why = "玩家挂单成交未开启"
                elif unit > hi:
                    why = "单价超出价格带上限（护栏）"
                elif uc.get("max_price_ratio") is not None and unit >= min(self.items[iid]["ref"], (plan["items"].get(iid) or {}).get("fair") or self.items[iid]["ref"]) * float(uc["max_price_ratio"]):
                    why = "单价不低于 min(参考价, 公允价)×%s" % uc["max_price_ratio"]
                elif uc.get("probability") is not None and random.random() >= float(uc["probability"]):
                    why = "未通过概率抽签"
                elif qty < 1:
                    why = "超出每日金币上限或单笔上限"
                if why:
                    b.log("buy", "模型想买玩家挂单 %s ×%d @%d（拍卖号 %d），未执行：%s" % (b.item_name(iid), f["qty"], unit, aid, why))
                    continue
                r = await b.client.buy_apiece(FAKE_BUYER, aid, unit, qty)
                if r.get("ok"):
                    user_gold_today += cost
                    res["uf_done"] += 1
                    res["user_gold"] += cost
                    b.store.add_deal(time.time(), iid, aid, qty, unit, cost, "bot_buy_human", None, FAKE_BUYER, "engine",
                                     "eng-u:%d:%d:%d" % (aid, qty, int(time.time() * 1000)))
                    b.log("buy", "模型成交玩家挂单 %s ×%d，单价 %d，花费 %d（拍卖号 %d，卖家 %s）"
                          % (b.item_name(iid), qty, unit, cost, aid, f.get("owner_name", "")), auction_id=aid)
                else:
                    res["errors"] += 1
                    b.log("error", "成交玩家挂单失败 拍卖号 %d：%s" % (aid, r.get("because_text")), level="warn")
                continue
            # 系统挂单：虚拟玩家之间的成交 → 假买家买下机器人自己的 -1 挂单
            res["vf_planned"] += 1
            if f["auction_id"]:
                aid = f["auction_id"]
                row = b.store.query("SELECT count_now, unit_price FROM bot_listings WHERE auction_id=? AND status='active'", (aid,))
                if not row:
                    continue
                have, unit = row[0]["count_now"], row[0]["unit_price"]
            else:
                if f["new_idx"] not in new_ids:
                    continue
                aid, have, unit = new_ids[f["new_idx"]]
                row = b.store.query("SELECT count_now FROM bot_listings WHERE auction_id=?", (aid,))
                have = row[0]["count_now"] if row else 0
            if res["vf_done"] >= vf_step_cap or vf_today >= vf_day_cap:
                res["skipped"].append("虚拟成交上限")
                continue
            qty = min(int(f["qty"]), have)
            if qty < 1:
                continue
            r = await b.client.buy_apiece(FAKE_BUYER, aid, unit, qty)
            if r.get("ok"):
                vf_today += 1
                res["vf_done"] += 1
                left = have - qty
                b.store.execute("UPDATE bot_listings SET count_now=?, status=?, closed_at=? WHERE auction_id=?",
                                (left, "active" if left > 0 else "sold", None if left > 0 else time.time(), aid))
                b.store.add_deal(time.time(), iid, aid, qty, unit, unit * qty, "virtual", -1, FAKE_BUYER, "engine",
                                 "eng-v:%d:%d:%d" % (aid, left, int(time.time() * 1000)))
                b.log("deal", "虚拟成交 %s ×%d，单价 %d（拍卖号 %d，%s）" % (
                    b.item_name(iid), qty, unit, aid,
                    "、".join("%s%d" % ({"consumer": "消费者", "flipper": "倒爷"}.get(k, k), v) for k, v in f["buyers"].items())),
                    level="debug", auction_id=aid)
            else:
                res["errors"] += 1
                b.log("error", "虚拟成交失败 拍卖号 %d ×%d：%s" % (aid, qty, r.get("because_text")), level="warn")
                if r.get("because") in (0x9e, 0x31):
                    break
        # ---- 3) 记录 ----
        b.store.kv_set("engine_state", self.engine.state)
        ts = time.time()
        b.store.execute("INSERT INTO engine_steps(ts,dt_h,online,items,new_planned,new_done,vf_planned,vf_done,uf_planned,"
                        "uf_done,user_gold,errors,elapsed) VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)",
                        (ts, plan["dt_h"], plan["online"], len(self.items), res["new_planned"], res["new_done"],
                         res["vf_planned"], res["vf_done"], res["uf_planned"], res["uf_done"], res["user_gold"],
                         res["errors"], ts - now))
        b.store.executemany("INSERT INTO engine_snap(ts,item_id,fair,stock,scarcity,x,n_sys,new,vqty,uqty) VALUES(?,?,?,?,?,?,?,?,?,?)",
                            [(ts, iid, i["fair"], i["stock"], i["scarcity"], i["x"], i["n_sys"], i["new"], i["virtual_qty"],
                              i["user_qty"]) for iid, i in plan["items"].items()])
        self.last_plan = plan
        b.log("engine", "模型步进：dt=%.1f 分钟，虚拟在线 %d；上架 %d/%d，虚拟成交 %d/%d，玩家挂单成交 %d/%d（%d 金），错误 %d，耗时 %.1fs%s"
              % (plan["dt_h"] * 60, plan["online"], res["new_done"], res["new_planned"], res["vf_done"], res["vf_planned"],
                 res["uf_done"], res["uf_planned"], res["user_gold"], res["errors"], ts - now,
                 ("；" + "、".join(sorted(set(res["skipped"])))) if res["skipped"] else ""))
        return res

    def status(self):
        b = self.bot
        c = self.cfg
        counts, nl, t0 = self.today_counts()
        info = (self.last_plan or {}).get("items", {})
        act = {r["item_id"]: r for r in b.store.query(
            "SELECT item_id, COUNT(*) n, SUM(count_now) q FROM bot_listings WHERE status='active' GROUP BY item_id")}
        dl = {}
        for r in b.store.query("SELECT item_id, side, COUNT(*) n, SUM(count) q, SUM(total) g FROM deals WHERE ts>=? "
                               "GROUP BY item_id, side", (t0,)):
            dl.setdefault(r["item_id"], {})[r["side"]] = r
        user_ls = {}
        npc = b.store.kv_get("npc_name")
        for iid, ls in b.snapshot.items():
            user_ls[iid] = sum(1 for l in ls if not b.is_npc_name(l["owner_name"]))
        rows = []
        for iid, p in self.items.items():
            st = self.engine.state.get(str(iid), {}) if self.engine else {}
            fair = self.engine.fair(iid)[0] if self.engine else None
            d = dl.get(iid, {})
            rows.append({"item_id": iid, "name": b.item_name(iid), "category": p.get("category"), "ref": p["ref"],
                         "band": p.get("band"), "fair": fair, "stock": st.get("stock"),
                         "cover_h": (st.get("stock") or 0) / max(p["faucet_per_h"], 1e-9),
                         "scarcity": (info.get(iid) or {}).get("scarcity"), "x": st.get("x"), "last": st.get("last"),
                         "sys_listings": (act.get(iid) or {}).get("n", 0), "sys_qty": (act.get(iid) or {}).get("q") or 0,
                         "user_listings": user_ls.get(iid, 0),
                         "virtual_today": (d.get("virtual") or {}).get("q") or 0,
                         "virtual_trades_today": (d.get("virtual") or {}).get("n") or 0,
                         "user_fill_today": (d.get("bot_buy_human") or {}).get("q") or 0,
                         "human_buy_today": (d.get("human_buy_bot") or {}).get("q") or 0,
                         "disabled": b.disabled_items.get(iid)})
        steps = b.store.query("SELECT * FROM engine_steps ORDER BY ts DESC LIMIT 30")
        vf = (counts.get("virtual") or {}).get("n") or 0
        return {"enabled": self.engine is not None, "volume_scale": c.get("volume_scale", 1.0),
                "step_minutes": c.get("step_minutes", [5, 15]), "next_step_at": self.next_step_at,
                "last_step_at": self.last_step_at, "items": rows,
                "skipped": [{"item_id": k, "name": b.item_name(k), "reason": v} for k, v in sorted(self.skipped.items())],
                "excluded": [{"item_id": k, "name": b.item_name(k), "reason": v} for k, v in sorted(self.excluded.items())],
                "today": {"listings": nl, "virtual_trades": vf,
                          "virtual_qty": (counts.get("virtual") or {}).get("q") or 0,
                          "virtual_gold": (counts.get("virtual") or {}).get("g") or 0,
                          "user_fills": (counts.get("bot_buy_human") or {}).get("n") or 0,
                          "user_gold": (counts.get("bot_buy_human") or {}).get("g") or 0,
                          "human_buys": (counts.get("human_buy_bot") or {}).get("n") or 0,
                          "orphan_rows_est": 2 * (vf + ((counts.get("bot_buy_human") or {}).get("n") or 0))},
                "caps": {"virtual_per_day": int((c.get("virtual_fill") or {}).get("max_per_day", 5000)),
                         "virtual_per_step": int((c.get("virtual_fill") or {}).get("max_per_step", 60)),
                         "user_fill_enabled": bool((c.get("user_fill") or {}).get("enabled", False)),
                         "user_gold_per_day": int((c.get("user_fill") or {}).get("max_gold_per_day", 2000000)),
                         "user_gold_per_buy": int((c.get("user_fill") or {}).get("max_gold_per_buy", (c.get("user_fill") or {}).get("max_gold_per_day", 2000000))),
                         "new_per_step": int(c.get("max_new_listings_per_step", 40)),
                         "new_per_hour": int(c.get("max_new_listings_per_hour", 200))},
                "steps": steps}
