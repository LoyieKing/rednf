# -*- coding: utf-8 -*-
"""
装备挂单（经济模型 v4.1 的 equipment 段）。

格式依据（ORIG df_game_r / 拍卖服，详见 README「装备挂单」）：
  * 游戏端登记拍卖（Inter_AuctionResultAskRegistedItemNum::dispatch_sig）把背包格子 Inven_Item 原样拷进 DnfItemInfo：
    seal=格子封装标记、add_info=m_addInfo2（装备是 make_item 时的 rand() 随机种子，不是数量）、endurance=当前耐久、
    guid=增幅区里的 itemGloballyUniqueIdentifier（serverId/datetime/seq）、random_option=格子随机属性。外层 guid@139 永远 0。
  * CEquipItem::make_item：耐久 = PVF [durability]（没有该字段 = 0）；[attach type]=[sealing] 时封装=1；
    装备类型 10..21（武器~魔法石）生成 guid；随机属性在「随机属性解封」时才生成。
  * 拍卖服 → 买家邮件（HandlerFor_DB_::insertPackageData）逐字段写 postal，取件时原样还原进背包。
所以机器人挂的装备 = 一件刚掉落、未解封的 +0 装备：seal=1，耐久满，add_info 随机，guid 唯一，随机属性全 0。
只挂 PVF 里没有 [random option] 的装备（运营 csv 标记与 PVF 双重核对）。
"""
import math
import random
import time

from . import protocol as P
from .engine import Engine, round_price

FAKE_BUYER = 1999999999
ATTACH_SEAL = {"[sealing]": 1, "[free]": 0, "[sealing trade]": 0}   # 拍卖服/游戏端允许登记的 attach type
GUID_SEQ_MIN = 60000          # 游戏端 getSeq 在 0xea5f(59999) 处归零，60000..65535 永远不会与游戏生成的 guid 重复
GUID_SEQ_MAX = 0xffff


def tier_key(t):
    return t if str(t).startswith("EQ_") else "EQ_" + str(t)


class EquipRunner:
    def __init__(self, runner):
        self.runner = runner
        self.bot = runner.bot
        self.catalog = {}         # iid -> {tier, ref, band, weight, seal, endurance}
        self.skipped = {}         # iid -> 原因
        self.tiers = {}           # tier -> params（engine 用）
        self.engine = None
        self.expired_pending = {}
        self.hourly_new = []
        self.rng = random.Random()
        self.last_plan = None
        self._guid_last = (0, GUID_SEQ_MIN - 1)

    # ---------------- 配置 ----------------
    @property
    def cfg(self):
        return (self.runner.cfg.get("equipment") or {})

    @property
    def listing_enabled(self):
        c = self.cfg
        return bool(c.get("enabled")) and bool(c.get("format_verified"))

    @property
    def manual_enabled(self):
        c = self.cfg
        return bool(c.get("manual_enabled")) and bool(c.get("format_verified"))

    def reload(self, model):
        b = self.bot
        self.catalog, self.skipped, self.tiers, self.engine = {}, {}, {}, None
        eq = (model or {}).get("equipment") or {}
        tiers = eq.get("tiers") or {}
        for k, it in (eq.get("items") or {}).items():
            iid = int(k)
            why = self._check(iid, it, tiers)
            if why:
                self.skipped[iid] = why
                continue
            meta = b.item_meta(iid)
            lst = it.get("listing") or {}
            self.catalog[iid] = {
                "tier": tier_key(it["tier"]), "ref": float(it["ref"]),
                "band": [float(x) for x in (it.get("band") or [it["ref"] * 0.8, it["ref"] * 1.25])],
                "weight": max(float(it.get("supply_weight") or 0), 1e-6),
                "seal": ATTACH_SEAL[meta["attach"]], "endurance": int(meta.get("durability") or 0),
                "model_endurance": lst.get("endurance"), "name": it.get("name") or b.item_name(iid)}
        maxpt = self.cfg.get("max_listings_per_tier")
        for t, p in tiers.items():
            if not any(c["tier"] == t for c in self.catalog.values()):
                continue
            p = dict(p)
            p["target_listings"] = int(p.get("target_listings", 2))
            if maxpt is not None:
                p["target_listings"] = max(0, min(p["target_listings"], int(maxpt)))
            p.setdefault("lot_qty", {"median": 1, "sigma": 0, "min": 1, "max": 1})
            p["lot_qty"] = {"median": 1, "sigma": 0, "min": 1, "max": 1}
            p["consumer_qty"] = {"median": 1, "sigma": 0, "min": 1, "max": 1}
            self.tiers[t] = p
        if self.tiers:
            state = b.store.kv_get("equip_engine_state") or {}
            ecfg = dict(self.runner.cfg)
            ecfg["listing_cap_scale"] = 0.5          # 层级挂单上限 = target_listings（引擎默认是 ×2）
            ecfg["volume_scale"] = float(self.cfg.get("volume_scale", 1.0))
            self.engine = Engine({"global": self.runner.model["global"]}, self.tiers, ecfg, state=state, rng=self.rng)
        if self.listing_enabled:
            b.watch_ids |= set(self.catalog)
        b.log("engine", "装备模型已加载：%d 件可挂（%d 个层级），跳过 %d 件；装备挂单%s，手动上架装备%s，收购玩家装备%s"
              % (len(self.catalog), len(self.tiers), len(self.skipped),
                 "已开启" if self.listing_enabled else "关闭", "允许" if self.manual_enabled else "关闭",
                 "已开启" if (self.cfg.get("user_fill") or {}).get("enabled") else "关闭"))

    def _check(self, iid, it, tiers):
        b = self.bot
        if iid in b.excluded:
            return "排除名单：" + b.excluded[iid]
        if tier_key(it.get("tier")) not in tiers:
            return "层级不存在：%s" % it.get("tier")
        if it.get("random_option_in_pvf") is not False:
            return "随机属性装备（假紫，random_option_in_pvf≠false），永不挂/买"
        lst = it.get("listing") or {}
        if int(lst.get("upgrade") or 0) != 0 or lst.get("random_option") not in (None, "none", "") \
                or int(lst.get("separate_upgrade") or 0) != 0:
            return "只挂 +0、无随机属性、无分离强化的装备"
        meta = b.item_meta(iid)
        if not meta:
            return "PVF 中找不到"
        if meta.get("kind") != "equipment" or meta.get("is_avatar"):
            return "PVF 中不是装备（或是时装）"
        if meta.get("random_option"):
            return "PVF 带 [random option]（假紫），永不挂/买"
        if meta.get("attach") not in ATTACH_SEAL:
            return "attach type %s 不能登记拍卖" % meta.get("attach")
        if float(it.get("ref") or 0) <= 0:
            return "没有参考价"
        return None

    # ---------------- 工具 ----------------
    def tier_of(self, iid):
        c = self.catalog.get(int(iid))
        return c["tier"] if c else None

    def band(self, iid):
        c = self.catalog[iid]
        s = float(self.cfg.get("price_band_slack", 0.0))
        lo, hi = c["band"]
        return lo * (1 - s), hi * (1 + s)

    def item_price(self, iid, tier_px):
        """层级价 → 单件价：items[id].ref × (层级价 / 层级 ref)，按价位取整，夹在单件价格带内，且不超过 int32/MAX_PRICE。"""
        from .bot import MAX_PRICE
        c = self.catalog[iid]
        t = self.tiers[c["tier"]]
        px = round_price(float(tier_px) * c["ref"] / float(t["ref"]))
        lo, hi = self.band(iid)
        px = int(min(max(px, math.ceil(lo)), math.floor(hi)))
        return max(1, min(px, MAX_PRICE, P.INT32_MAX))

    def new_guid(self):
        """唯一 guid：serverId = 本服 gcNo（库里真实装备是 3011），datetime = 当前时间，
        seq 取 60000..65535（游戏端序号 ≤59999，永不重复）；同一秒内递增。"""
        sid = int(self.cfg.get("guid_server_id", 3011))
        now = int(time.time())
        lt, ls = self._guid_last
        st = self.bot.store.kv_get("equip_guid_last") or [0, GUID_SEQ_MIN - 1]
        if st[0] > lt or (st[0] == lt and st[1] > ls):
            lt, ls = st
        if now > lt:
            seq = GUID_SEQ_MIN + self.rng.randint(0, 999)
        else:
            now = lt
            seq = ls + 1
            if seq > GUID_SEQ_MAX:
                now, seq = lt + 1, GUID_SEQ_MIN
        self._guid_last = (now, seq)
        self.bot.store.kv_set("equip_guid_last", [now, seq])
        return P.make_item_guid(sid, now, seq), (sid, now, seq)

    def item_info(self, iid):
        """一件刚掉落、未解封的 +0 装备（见模块说明）。返回 (53 字节, 字段说明)。"""
        c = self.catalog[iid]
        add_info = self.rng.randint(0, 0x7fffffff)          # ORIG make_item：set_add_info(rand())
        guid, g = self.new_guid()
        info = P.equipment_item_info(iid, c["seal"], c["endurance"], add_info, guid)
        return info, {"seal": c["seal"], "endurance": c["endurance"], "add_info": add_info, "guid": g}

    async def ensure_ok(self, iid):
        b = self.bot
        if iid not in self.catalog or iid in b.disabled_items:
            return False
        if b.store.kv_get("probe_ok:%d" % iid):
            return True
        n = await b.client.probe_item(iid)
        if n == -2:
            b.disabled_items[iid] = "拍卖服 iteminfo.dat 中没有此物品"
            b.log("error", "装备 %d %s：%s" % (iid, b.item_name(iid), b.disabled_items[iid]), level="warn")
            return False
        if n == -1:
            return False
        b.store.kv_set("probe_ok:%d" % iid, True)
        return True

    async def list_one(self, iid, price, note):
        """以系统卖家（-1）挂 1 件装备，写 bot_listings。返回 (结果, 字段)。"""
        b = self.bot
        info, f = self.item_info(iid)
        r = await b.client.regist_equipment(-1, "", iid, int(price), info)
        if r["ok"] and r["auction_id"]:
            t = time.time()
            b.store.execute("INSERT OR REPLACE INTO bot_listings(auction_id,item_id,count0,count_now,unit_price,instant_price,"
                            "listed_at,expire_at,status,last_seen,note) VALUES(?,?,1,1,?,?,?,?,'active',?,?)",
                            (r["auction_id"], iid, int(price), int(price), t, t + 7200, t, note))
        return r, f

    @staticmethod
    def plain_plus0(item):
        """检索回包里的物品是不是「+0、无封装次数、无随机属性、无分离强化」的普通装备。"""
        return (item.get("upgrade") == 0 and item.get("seal_cnt") == 0 and item.get("random_empty", True)
                and not item.get("separate_raw") and not item.get("ability_type") and not item.get("ability_value"))

    def day_spent_user(self, t0):
        ids = list(self.catalog) or [0]
        r = self.bot.store.query("SELECT SUM(total) g FROM deals WHERE ts>=? AND side='bot_buy_human' AND item_id IN (%s)"
                                 % ",".join(map(str, ids)), (t0,))
        return (r[0]["g"] if r else 0) or 0

    # ---------------- 模型一步 ----------------
    async def step(self, now, dt_h):
        b = self.bot
        c = self.cfg
        res = {"new_planned": 0, "new_done": 0, "vf_done": 0, "uf_done": 0, "errors": 0, "user_gold": 0}
        if self.engine is None or not self.listing_enabled:
            return res
        mine = {r["auction_id"]: r for r in b.store.query("SELECT auction_id, note FROM bot_listings WHERE status='active'")}
        book, where = {}, {}
        for iid, ci in self.catalog.items():
            t = self.tiers[ci["tier"]]
            for l in b.snapshot.get(iid, []):
                aid = l["auction_id"]
                row = mine.get(aid)
                if row is not None and str(row.get("note") or "").startswith("manual"):
                    continue
                if not self.plain_plus0(l["item"]) or l["instant_price"] <= 0:
                    continue
                is_sys = aid in mine or b.is_npc_name(l["owner_name"])
                norm = l["instant_price"] * float(t["ref"]) / ci["ref"]
                book.setdefault(ci["tier"], []).append({
                    "auction_id": aid, "price": norm, "qty": 1, "owner_id": -1 if is_sys else 0,
                    "seller": "sys" if is_sys else "user", "buyable": (aid in mine) if is_sys else True,
                    "owner_name": l["owner_name"]})
                where[aid] = (iid, l["instant_price"], l["owner_name"])
        expired, self.expired_pending = self.expired_pending, {}
        plan = self.engine.step(dt_h, book, expired, now)
        res["new_planned"] = len(plan["new"])
        self.hourly_new = [x for x in self.hourly_new if now - x < 3600]
        per_step, per_hour = int(c.get("max_new_per_step", 2)), int(c.get("max_new_per_hour", 8))
        new_ids = {}
        for idx, n in enumerate(plan["new"]):
            if res["new_done"] >= per_step or len(self.hourly_new) >= per_hour:
                break
            cands = [i for i, ci in self.catalog.items() if ci["tier"] == n["item_id"] and i not in b.disabled_items]
            if not cands:
                continue
            iid = self.rng.choices(cands, weights=[self.catalog[i]["weight"] for i in cands])[0]
            if not await self.ensure_ok(iid):
                continue
            px = self.item_price(iid, n["price"])
            r, f = await self.list_one(iid, px, "eq:" + n["seller"])
            if not r["ok"] or not r["auction_id"]:
                res["errors"] += 1
                b.log("error", "装备上架失败 %s @%d：%s" % (b.item_name(iid), px, r.get("because_text") or "拍卖号为 0"), level="warn")
                if r.get("because") in (0x9e, 0x31):
                    break
                continue
            aid = r["auction_id"]
            new_ids[idx] = (aid, iid, px)
            self.hourly_new.append(time.time())
            res["new_done"] += 1
            b.log("list", "装备上架[%s] %s（%s）一口价 %d（拍卖号 %d，封装 %d，耐久 %d）" % (
                n["seller"], b.item_name(iid), n["item_id"], px, aid, f["seal"], f["endurance"]), auction_id=aid, item_id=iid)
        # 成交
        _, _, t0 = self.runner.today_counts()
        vc = c.get("virtual_fill") or {}
        uc = c.get("user_fill") or {}
        vf_today = b.store.query("SELECT COUNT(*) n FROM deals WHERE ts>=? AND side='virtual' AND source='engine-eq'", (t0,))[0]["n"]
        spent = self.day_spent_user(t0)
        buyer_name = str((b.buyer_cfg or {}).get("buyer_name", "收购商"))
        for f in sorted(plan["fills"], key=lambda x: (not x["user"],)):
            if f["auction_id"]:
                aid = f["auction_id"]
                if aid not in where:
                    continue
                iid, inst, owner = where[aid]
            elif f.get("new_idx") in new_ids:
                aid, iid, inst = new_ids[f["new_idx"]]
                owner = ""
            else:
                continue
            if iid not in self.catalog:
                continue
            if f["user"]:
                lo, hi = self.band(iid)
                why = None
                if not uc.get("enabled", False):
                    why = "收购玩家装备未开启（engine.equipment.user_fill.enabled）"
                elif inst > hi:
                    why = "一口价高于价格带上限"
                elif spent + inst > int(uc.get("max_gold_per_day", 0)):
                    why = "超出装备收购每日金币上限"
                if why:
                    b.log("buy", "模型想买玩家装备 %s 一口价 %d（拍卖号 %d），未执行：%s" % (b.item_name(iid), inst, aid, why))
                    continue
                live = [l for l in await b.client.search_items([iid]) if l["auction_id"] == aid]
                if not live or not self.plain_plus0(live[0]["item"]) or live[0]["instant_price"] != inst:
                    b.log("buy", "玩家装备挂单 %d 已变化或不是 +0 普通装备，跳过" % aid)
                    continue
                r = await b.client.buy_now(FAKE_BUYER, buyer_name, aid, inst)
                if r.get("ok"):
                    spent += inst
                    res["uf_done"] += 1
                    res["user_gold"] += inst
                    b.store.add_deal(time.time(), iid, aid, 1, inst, inst, "bot_buy_human", None, FAKE_BUYER, "engine-eq",
                                     "eq-u:%d:%d" % (aid, int(time.time() * 1000)))
                    b.log("buy", "模型收购玩家装备 %s，一口价 %d（拍卖号 %d，卖家 %s）" % (b.item_name(iid), inst, aid, owner), auction_id=aid)
                else:
                    res["errors"] += 1
                    b.log("error", "收购玩家装备失败 拍卖号 %d：%s" % (aid, r.get("because_text")), level="warn")
                continue
            if res["vf_done"] >= int(vc.get("max_per_step", 4)) or vf_today >= int(vc.get("max_per_day", 60)):
                continue
            row = b.store.query("SELECT count_now FROM bot_listings WHERE auction_id=? AND status='active'", (aid,))
            if not row or row[0]["count_now"] < 1:
                continue
            r = await b.client.buy_now(FAKE_BUYER, buyer_name, aid, inst)
            if r.get("ok"):
                vf_today += 1
                res["vf_done"] += 1
                b.store.execute("UPDATE bot_listings SET count_now=0, status='sold', closed_at=? WHERE auction_id=?", (time.time(), aid))
                b.store.add_deal(time.time(), iid, aid, 1, inst, inst, "virtual", -1, FAKE_BUYER, "engine-eq",
                                 "eq-v:%d:%d" % (aid, int(time.time() * 1000)))
                b.log("deal", "装备虚拟成交 %s 一口价 %d（拍卖号 %d）" % (b.item_name(iid), inst, aid), level="debug", auction_id=aid)
            else:
                res["errors"] += 1
                b.log("error", "装备虚拟成交失败 拍卖号 %d：%s" % (aid, r.get("because_text")), level="warn")
        b.store.kv_set("equip_engine_state", self.engine.state)
        self.last_plan = plan
        b.log("engine", "装备模型步进：上架 %d/%d，虚拟成交 %d，收购玩家装备 %d（%d 金），错误 %d"
              % (res["new_done"], res["new_planned"], res["vf_done"], res["uf_done"], res["user_gold"], res["errors"]))
        return res

    # ---------------- 状态 ----------------
    def status(self):
        b = self.bot
        c = self.cfg
        ids = list(self.catalog) or [0]
        act = {}
        for r in b.store.query("SELECT item_id, COUNT(*) n FROM bot_listings WHERE status='active' AND COALESCE(note,'') NOT LIKE 'manual%%' "
                               "AND item_id IN (%s) GROUP BY item_id" % ",".join(map(str, ids))):
            t = self.tier_of(r["item_id"])
            act[t] = act.get(t, 0) + r["n"]
        _, _, t0 = self.runner.today_counts()
        vd = {}
        for r in b.store.query("SELECT item_id, COUNT(*) n, SUM(total) g FROM deals WHERE ts>=? AND source='engine-eq' AND side='virtual' "
                               "GROUP BY item_id", (t0,)):
            t = self.tier_of(r["item_id"])
            vd[t] = vd.get(t, 0) + r["n"]
        tiers = []
        for t, p in sorted(self.tiers.items(), key=lambda x: x[1]["ref"]):
            fair = self.engine.fair(t)[0] if self.engine else None
            tiers.append({"tier": t, "name": p.get("name"), "ref": p["ref"], "band": p.get("band"), "fair": fair,
                          "target": p["target_listings"], "items": sum(1 for x in self.catalog.values() if x["tier"] == t),
                          "sys_listings": act.get(t, 0), "virtual_today": vd.get(t, 0)})
        reasons = {}
        for v in self.skipped.values():
            k = v.split("：")[0]
            reasons[k] = reasons.get(k, 0) + 1
        uc = c.get("user_fill") or {}
        return {"listing_enabled": self.listing_enabled, "format_verified": bool(c.get("format_verified")),
                "manual_enabled": self.manual_enabled, "items": len(self.catalog), "tiers": tiers,
                "skipped": reasons, "user_fill_enabled": bool(uc.get("enabled", False)),
                "user_gold_per_day": int(uc.get("max_gold_per_day", 0)), "user_gold_today": self.day_spent_user(t0),
                "max_listings_per_tier": c.get("max_listings_per_tier")}
