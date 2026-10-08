# -*- coding: utf-8 -*-
"""机器人主循环：自动上架（系统 -1 卖家）、可选自动收购、行情采集、孤儿邮件清理。"""
import asyncio
import csv
import math
import os
import random
import time
import traceback

from . import protocol as P
from .client import AuctionClient
from .gamedb import GameDB
from .store import Store

SYSTEM_OWNER = -1
SYSTEM_EXPIRE_SEC = 7200          # 拍卖服硬编码（Auction 构造函数 mSYSTEM_AUCTION_EXPIRE_TIME，不可配置）
MAX_PRICE = 2000000000


def load_reference_prices(path):
    ref = {}
    if not path or not os.path.exists(path):
        return ref
    with open(path, encoding="utf-8-sig") as f:
        for row in csv.DictReader(f):
            try:
                iid = int(row.get("道具ID") or row.get("item_id") or row.get("物品ID") or 0)
            except ValueError:
                continue
            if not iid:
                continue

            def num(*keys):
                for k in keys:
                    v = row.get(k)
                    if v not in (None, ""):
                        try:
                            return int(float(v))
                        except ValueError:
                            pass
                return None
            ref[iid] = {"name": row.get("道具名") or row.get("name") or "",
                        "ref": num("参考价(金币)", "ref_price", "参考价"),
                        "low": num("下限(金币)", "low", "下限"),
                        "high": num("上限(金币)", "high", "上限")}
    return ref


class Bot:
    def __init__(self, cfg, base_dir, items_index):
        self.cfg = cfg
        self.base_dir = base_dir
        self.items = items_index or {}
        data_dir = os.path.join(base_dir, cfg.get("data_dir", "data"))
        os.makedirs(data_dir, exist_ok=True)
        self.store = Store(os.path.join(data_dir, "bot.db"))
        self.db = GameDB(cfg.get("game_db", {}), self.log)
        a = cfg.get("auction_server", {})
        self.client = AuctionClient(a.get("host", "127.0.0.1"), int(a.get("port", 30803)), self.log,
                                    float(a.get("request_timeout", 8)), float(a.get("min_interval", 0.15)))
        self.loop = None
        self.paused = bool(cfg.get("start_paused", False))
        self.started_at = time.time()
        self.last = {}                    # 各任务上次执行时间
        self.snapshot = {}                # item_id -> [listing dict]（最近一次协议检索）
        self.snapshot_ts = 0
        self.ref = {}
        self.disabled_items = {}          # item_id -> 原因
        self.hourly = []                  # 最近一小时上架时间戳
        self.buy_hist = []                # (ts, gold)
        self.reload_config(cfg)
        self._stop = None

    # ---------------- 工具 ----------------
    def log(self, kind, msg, level="info", **extra):
        try:
            self.store.log(level, kind, msg, extra or None)
        except Exception:
            pass
        print("%s [%s] %s: %s" % (time.strftime("%H:%M:%S"), level, kind, msg), flush=True)

    def item_name(self, iid):
        it = self.items.get(int(iid))
        if it and it.get("name"):
            return it.get("name") or str(iid)
        r = self.ref.get(int(iid))
        return (r or {}).get("name") or str(iid)

    def item_meta(self, iid):
        m = self.items.get(int(iid)) or {}
        if m and "is_avatar" not in m:
            m["is_avatar"] = "avatar" in (m.get("path") or "") or "avatar" in str(m.get("equipment_type") or "")
        return m

    def reload_config(self, cfg):
        self.cfg = cfg
        csv_path = cfg.get("reference_prices_csv")
        if csv_path and not os.path.isabs(csv_path):
            csv_path = os.path.join(self.base_dir, csv_path)
        self.ref = load_reference_prices(csv_path)
        self.seller_items = {}
        for it in (cfg.get("seller", {}).get("items") or []):
            if not it.get("enabled", True):
                continue
            iid = int(it["item_id"])
            base = it.get("base_price") or (self.ref.get(iid) or {}).get("ref")
            if not base:
                self.log("error", "上架物品 %d 没有基础价格（配置和参考价表都没有），已跳过" % iid, level="warn")
                continue
            self.seller_items[iid] = dict(it, base_price=int(base))
        self.buyer_cfg = cfg.get("buyer", {}) or {}
        self.watch_ids = set(self.seller_items)
        if cfg.get("market", {}).get("watch_reference_items", True):
            self.watch_ids |= set(self.ref)
        for x in cfg.get("market", {}).get("extra_watch_items", []) or []:
            self.watch_ids.add(int(x))

    def base_price(self, iid):
        if iid in self.seller_items:
            return self.seller_items[iid]["base_price"]
        b = (self.buyer_cfg.get("base_prices") or {}).get(iid) or (self.buyer_cfg.get("base_prices") or {}).get(str(iid))
        if b:
            return int(b)
        return (self.ref.get(iid) or {}).get("ref")

    # ---------------- 主循环 ----------------
    async def run(self):
        self.loop = asyncio.get_running_loop()
        self._stop = asyncio.Event()
        self.log("connect", "机器人启动（版本 %s）" % __import__("auction_bot").__version__)
        self._check_buyer_id()
        backoff = 5
        while not self._stop.is_set():
            try:
                await self.client.connect()
                backoff = 5
                self.store.kv_set("last_connect", time.time())
                await self._session()
            except asyncio.CancelledError:
                break
            except Exception as e:
                self.log("error", "会话异常：%s" % e, level="error", tb=traceback.format_exc()[-1500:])
            await self.client.close()
            if self._stop.is_set():
                break
            self.client.stats["reconnects"] += 1
            self.log("connect", "%d 秒后重连" % backoff, level="warn")
            try:
                await asyncio.wait_for(self._stop.wait(), backoff)
            except asyncio.TimeoutError:
                pass
            backoff = min(backoff * 2, 300)
        await self.client.close()
        self.log("connect", "机器人已停止")

    def stop(self):
        if self._stop:
            self.loop.call_soon_threadsafe(self._stop.set)

    def _check_buyer_id(self):
        bid = int(self.buyer_cfg.get("buyer_id", 1999999999))
        try:
            if self.db.charac_exists(bid):
                self.log("error", "收购用的假角色 ID %d 在 charac_info 里真实存在！自动收购已强制关闭" % bid, level="error")
                self.buyer_cfg["enabled"] = False
                self.buyer_cfg["_blocked"] = True
        except Exception as e:
            self.log("error", "检查收购角色 ID 失败：%s" % e, level="warn")

    def _due(self, name, interval):
        now = time.time()
        if now - self.last.get(name, 0) >= interval:
            self.last[name] = now
            return True
        return False

    async def _session(self):
        mk = self.cfg.get("market", {})
        se = self.cfg.get("seller", {})
        oc = self.cfg.get("orphan_cleanup", {})
        self.last.pop("scan", None)
        while not self._stop.is_set() and self.client.connected:
            try:
                if self.client.ready and self.client.service_available:
                    if self._due("scan", float(mk.get("scan_interval", 60))):
                        await self.scan_market()
                    if self._due("history", float(mk.get("history_interval", 30))):
                        await asyncio.to_thread(self.poll_history)
                    if not self.paused:
                        if se.get("enabled", True) and self._due("restock", float(se.get("restock_interval", 120))):
                            await self.restock()
                        if self.buyer_cfg.get("enabled") and self._due("buy", float(self.buyer_cfg.get("scan_interval", 120))):
                            await self.auto_buy()
                    if oc.get("enabled") and self._due("orphan", float(oc.get("interval", 3600))):
                        await asyncio.to_thread(self.orphan_cleanup_run, None)
                    if self._due("prune", 6 * 3600):
                        self.store.prune(int(mk.get("keep_days", 60)), int(mk.get("keep_log_days", 60)))
            except (ConnectionError, OSError):
                raise
            except asyncio.TimeoutError:
                self.log("error", "拍卖服请求超时（可能是参数被拒绝或拍卖服繁忙）", level="warn")
            except Exception as e:
                self.log("error", "任务异常：%s" % e, level="error", tb=traceback.format_exc()[-1500:])
            try:
                await asyncio.wait_for(self._stop.wait(), 2)
            except asyncio.TimeoutError:
                pass

    # ---------------- 行情采集 / 对账 ----------------
    async def scan_market(self):
        ids = sorted(self.watch_ids)
        if not ids:
            return
        listings = await self.client.search_items(ids)
        now = time.time()
        snap = {}
        for l in listings:
            snap.setdefault(l["item"]["item_id"], []).append(l)
        self.snapshot = snap
        self.snapshot_ts = now
        self.learn_npc_name()
        known = {r["auction_id"]: r for r in self.store.query("SELECT * FROM bot_listings WHERE status='active'")}
        obs = []
        npc_name = self.store.kv_get("npc_name")
        for l in listings:
            aid = l["auction_id"]
            cnt = l["item"]["add_info"] if l["item"]["add_info"] > 0 else 1
            unit = l["unit_price"] or (l["instant_price"] // cnt if l["instant_price"] > 0 else l["price"] // cnt)
            is_bot = 1 if aid in known or (npc_name and l["owner_name"] == npc_name) else 0
            obs.append((now, l["item"]["item_id"], aid, l["owner_name"], is_bot, unit, cnt))
        self.store.executemany("INSERT INTO listing_obs VALUES(?,?,?,?,?,?,?)", obs)
        # 对账：机器人挂单
        seen = {l["auction_id"]: l for l in listings}
        for aid, r in known.items():
            if r["item_id"] not in self.watch_ids:
                continue
            l = seen.get(aid)
            if l is not None:
                cnt = l["item"]["add_info"]
                if 0 < cnt < r["count_now"]:
                    sold = r["count_now"] - cnt
                    self.store.add_deal(now, r["item_id"], aid, sold, r["unit_price"], sold * r["unit_price"],
                                        "human_buy_bot", SYSTEM_OWNER, None, "snapshot", "snap:%d:%d" % (aid, cnt))
                    self.store.execute("UPDATE bot_listings SET count_now=?, last_seen=? WHERE auction_id=?", (cnt, now, aid))
                    self.log("deal", "玩家买走系统挂单 %s ×%d（单价 %d，拍卖号 %d，剩余 %d）"
                             % (self.item_name(r["item_id"]), sold, r["unit_price"], aid, cnt))
                else:
                    self.store.execute("UPDATE bot_listings SET last_seen=? WHERE auction_id=?", (now, aid))
                continue
            if now >= r["expire_at"] - 90:
                self.store.execute("UPDATE bot_listings SET status='expired', closed_at=? WHERE auction_id=?", (now, aid))
                self.log("expire", "系统挂单到期下架：%s ×%d（单价 %d，拍卖号 %d）"
                         % (self.item_name(r["item_id"]), r["count_now"], r["unit_price"], aid))
            else:
                self.store.execute("UPDATE bot_listings SET status='sold', closed_at=?, count_now=0 WHERE auction_id=?", (now, aid))
                self.store.add_deal(now, r["item_id"], aid, r["count_now"], r["unit_price"], r["count_now"] * r["unit_price"],
                                    "human_buy_bot", SYSTEM_OWNER, None, "snapshot", "snap:%d:0" % aid)
                self.log("deal", "玩家买走系统挂单 %s ×%d（单价 %d，拍卖号 %d，整单售罄）"
                         % (self.item_name(r["item_id"]), r["count_now"], r["unit_price"], aid))
        self.store.kv_set("last_scan", {"ts": now, "listings": len(listings), "items": len(ids)})
        self.log("scan", "行情扫描完成：%d 个物品，%d 条挂单（系统 %d）" % (len(ids), len(listings), sum(o[4] for o in obs)),
                 level="debug")

    def poll_history(self):
        """读取 auction_history_YYYYMM（只读）：补充玩家之间的成交、给系统挂单成交补买家 ID。"""
        last = self.store.kv_get("history_since", time.time() - 3 * 86400)
        bid = int(self.buyer_cfg.get("buyer_id", 1999999999))
        newest = last
        tables = self.db.history_tables()
        months = {time.strftime("auction_history_%Y%m", time.localtime(last)), time.strftime("auction_history_%Y%m")}
        for t in tables:
            if t not in months:
                continue
            for h in self.db.history_since(t, last - 5):
                newest = max(newest, h["ts"] or 0)
                if h["event_type"] == 0:                 # 0=到期下架
                    if h["owner_id"] == SYSTEM_OWNER:
                        cur = self.store.execute("UPDATE bot_listings SET status='expired' WHERE auction_id=? AND status='sold'",
                                                 (h["auction_id"],))
                        if cur.rowcount:
                            self.store.execute("DELETE FROM deals WHERE uniq=?", ("snap:%d:0" % h["auction_id"],))
                            self.log("expire", "按拍卖记录更正：拍卖号 %d 实为到期下架，非售出" % h["auction_id"])
                    continue
                cnt = h["add_info"] if h["add_info"] and h["add_info"] > 0 else 1
                unit = h["unit_price"] or (h["price"] // cnt if h["price"] else 0)
                if h["owner_id"] == SYSTEM_OWNER:
                    self.store.execute("UPDATE deals SET buyer_id=? WHERE auction_id=? AND buyer_id IS NULL",
                                       (h["buyer_id"], h["auction_id"]))
                    continue
                if h["buyer_id"] == bid:
                    continue
                if self.store.add_deal(h["ts"], h["item_id"], h["auction_id"], cnt, unit, h["price"] or 0, "human_human",
                                       h["owner_id"], h["buyer_id"], "history",
                                       "hist:%d:%d:%d" % (h["auction_id"], h["ts"], cnt)):
                    self.log("deal", "玩家间成交：%s ×%d，单价 %d（卖家 %s → 买家 %s）"
                             % (self.item_name(h["item_id"]), cnt, unit, h["owner_id"], h["buyer_id"]))
        self.store.kv_set("history_since", newest)

    # ---------------- 自动上架 ----------------
    def fair_value(self, iid):
        """每个物品的“公允价”：围绕基础价做均值回归随机游走（OU 过程），让行情有自然波动。"""
        it = self.seller_items[iid]
        base = it["base_price"]
        key = "fair:%d" % iid
        st = self.store.kv_get(key)
        now = time.time()
        drift = float(it.get("drift", self.cfg.get("seller", {}).get("default_drift", 0.05)))   # 每日对数波动
        theta = 1.0 / max(0.1, float(self.cfg.get("seller", {}).get("mean_revert_days", 3)))
        if not st or st.get("base") != base:
            x = 0.0
        else:
            dt = max(0.0, (now - st["ts"]) / 86400.0)
            x = st["x"] * math.exp(-theta * dt) + drift * math.sqrt(max(dt, 1e-9)) * random.gauss(0, 1)
            x = max(-0.5, min(0.5, x))
        self.store.kv_set(key, {"x": x, "ts": now, "base": base})
        return base * math.exp(x)

    def _price_for(self, iid):
        it = self.seller_items[iid]
        vol = float(it.get("volatility", self.cfg.get("seller", {}).get("default_volatility", 0.08)))
        p = self.fair_value(iid) * math.exp(random.gauss(0, vol))
        r = self.ref.get(iid) or {}
        lo = it.get("min_price") or (r.get("low") if self.cfg.get("seller", {}).get("clamp_to_reference_band", True) else None)
        hi = it.get("max_price") or (r.get("high") if self.cfg.get("seller", {}).get("clamp_to_reference_band", True) else None)
        if lo:
            p = max(p, lo)
        if hi:
            p = min(p, hi)
        return max(1, int(round(p)))

    async def ensure_item_ok(self, iid):
        if iid in self.disabled_items:
            return False
        if self.store.kv_get("probe_ok:%d" % iid):
            return True
        meta = self.item_meta(iid)
        if meta and meta.get("kind") != "stackable":
            self.disabled_items[iid] = "PVF 中不是消耗品/材料（stackable），机器人只上架可堆叠物品"
            self.log("error", "物品 %d %s：%s" % (iid, self.item_name(iid), self.disabled_items[iid]), level="warn")
            return False
        n = await self.client.probe_item(iid)
        if n == -2:
            self.disabled_items[iid] = "拍卖服 iteminfo.dat 中没有此物品（拍卖服会静默吞掉上架请求）"
            self.log("error", "物品 %d %s：%s" % (iid, self.item_name(iid), self.disabled_items[iid]), level="warn")
            return False
        if n == -1:
            raise asyncio.TimeoutError()   # DB 队列忙，稍后重试
        self.store.kv_set("probe_ok:%d" % iid, True)
        return True

    async def restock(self):
        se = self.cfg.get("seller", {})
        now = time.time()
        self.hourly = [t for t in self.hourly if now - t < 3600]
        per_cycle = int(se.get("max_new_listings_per_cycle", 3))
        per_hour = int(se.get("max_new_listings_per_hour", 60))
        made = 0
        active = {}
        for r in self.store.query("SELECT item_id, COUNT(*) n, SUM(count_now) q FROM bot_listings "
                                  "WHERE status='active' GROUP BY item_id"):
            active[r["item_id"]] = r
        order = list(self.seller_items)
        random.shuffle(order)
        stop = False
        for iid in order:
            if stop:
                break
            it = self.seller_items[iid]
            target = int(it.get("target_listings", 3))
            have = (active.get(iid) or {}).get("n", 0)
            while have < target and not stop:
                if made >= per_cycle or len(self.hourly) >= per_hour:
                    stop = True
                    break
                if not await self.ensure_item_ok(iid):
                    break
                meta = self.item_meta(iid)
                cmin = max(1, int(it.get("count_min", 1)))
                cmax = max(cmin, int(it.get("count_max", 10)))
                lim = int(meta.get("stack_limit") or 0)
                if lim > 0:
                    cmax = min(cmax, lim)
                    cmin = min(cmin, cmax)
                cnt = random.randint(cmin, cmax)
                unit = self._price_for(iid)
                if unit * cnt > MAX_PRICE:
                    cnt = max(1, MAX_PRICE // unit)
                res = await self.client.regist_item(SYSTEM_OWNER, "", iid, cnt, unit)
                if not res["ok"]:
                    self.log("error", "上架失败 %s ×%d 单价 %d：%s" % (self.item_name(iid), cnt, unit, res["because_text"]),
                             level="warn", because=res["because"])
                    stop = res["because"] in (0x9e, 0x31)
                    break
                aid = res["auction_id"]
                if not aid:
                    self.disabled_items[iid] = "拍卖服返回成功但拍卖号为 0（物品不在 iteminfo.dat），已停用"
                    self.log("error", "物品 %d %s：%s" % (iid, self.item_name(iid), self.disabled_items[iid]), level="error")
                    break
                now = time.time()
                self.store.execute("INSERT OR REPLACE INTO bot_listings(auction_id,item_id,count0,count_now,unit_price,"
                                   "instant_price,listed_at,expire_at,status,last_seen) VALUES(?,?,?,?,?,?,?,?,'active',?)",
                                   (aid, iid, cnt, cnt, unit, unit * cnt, now, now + SYSTEM_EXPIRE_SEC, now))
                self.hourly.append(now)
                made += 1
                have += 1
                self.log("list", "系统上架 %s ×%d，单价 %d，总价 %d（拍卖号 %d，2 小时后到期）"
                         % (self.item_name(iid), cnt, unit, unit * cnt, aid), auction_id=aid, item_id=iid)
        if made and not self.store.kv_get("npc_name"):
            self.last["scan"] = 0       # 下一轮马上扫描，学习系统卖家显示名

    def learn_npc_name(self):
        if self.store.kv_get("npc_name"):
            return
        mine = {r["auction_id"] for r in self.store.query("SELECT auction_id FROM bot_listings")}
        for ls in self.snapshot.values():
            for l in ls:
                if l["auction_id"] in mine and l["owner_name"]:
                    self.store.kv_set("npc_name", l["owner_name"])
                    self.log("scan", "系统卖家在拍卖行显示为「%s」" % l["owner_name"])
                    return

    # ---------------- 自动收购 ----------------
    async def auto_buy(self, force_auction_id=None, force_count=None):
        bc = self.buyer_cfg
        if bc.get("_blocked"):
            return []
        bid = int(bc.get("buyer_id", 1999999999))
        name = str(bc.get("buyer_name", "收购商"))
        now = time.time()
        self.buy_hist = [x for x in self.buy_hist if now - x[0] < 86400]
        spent_day = sum(g for _, g in self.buy_hist)
        buys_hour = sum(1 for t, _ in self.buy_hist if now - t < 3600)
        max_day = int(bc.get("max_gold_per_day", 2000000))
        max_hour = int(bc.get("max_buys_per_hour", 5))
        prem = float(bc.get("max_premium", 0.0))
        prob = float(bc.get("probability", 0.3))
        excl_owner = set(int(x) for x in bc.get("exclude_owner_ids", []) or [])
        excl_item = set(int(x) for x in bc.get("exclude_items", []) or [])
        where = "owner_id NOT IN (-1, %d) AND buyer_id IN (-1,0) AND instant_price > 0" % bid
        if force_auction_id:
            where += " AND auction_id=%d" % int(force_auction_id)
        cands = await asyncio.to_thread(self.db.auction_main, where)
        done = []
        for c in cands:
            iid = c["item_id"]
            if not force_auction_id:
                if buys_hour >= max_hour or spent_day >= max_day:
                    break
                if c["owner_id"] in excl_owner or iid in excl_item:
                    continue
                if bc.get("only_configured_items", True) and not self.base_price(iid):
                    continue
            meta = self.item_meta(iid)
            stackable = meta.get("kind") == "stackable"
            if meta.get("kind") not in ("stackable", "equipment") or meta.get("is_avatar"):
                continue
            # 以协议实时检索确认挂单当前状态
            live = [l for l in await self.client.search_items([iid]) if l["auction_id"] == c["auction_id"]]
            if not live:
                continue
            l = live[0]
            cnt_total = l["item"]["add_info"] if stackable and l["item"]["add_info"] > 0 else 1
            unit = l["unit_price"] or (l["instant_price"] // cnt_total)
            base = self.base_price(iid)
            if not force_auction_id:
                if not base or unit > base * (1 + prem):
                    continue
                if random.random() > prob:
                    self.log("buy", "收购候选 %s 单价 %d 未通过概率抽签，跳过" % (self.item_name(iid), unit), level="debug")
                    continue
            if stackable and l["price"] == -1 and l["unit_price"] > 0:
                n = min(cnt_total, int(force_count or bc.get("max_count_per_buy", 50)))
                n = max(1, min(n, max(1, (max_day - spent_day) // max(1, unit)) if not force_auction_id else n))
                cost = unit * n
                r = await self.client.buy_apiece(bid, l["auction_id"], unit, n)
                ok = r.get("ok")
                why = r.get("because_text")
            else:
                n = cnt_total
                cost = l["instant_price"]
                if not force_auction_id and spent_day + cost > max_day:
                    continue
                r = await self.client.buy_now(bid, name, l["auction_id"], cost)
                ok = r.get("ok")
                why = r.get("because_text")
            if ok:
                self.buy_hist.append((time.time(), cost))
                spent_day += cost
                buys_hour += 1
                self.store.add_deal(time.time(), iid, l["auction_id"], n, unit, cost, "bot_buy_human", c["owner_id"], bid,
                                    "bot", "buy:%d:%d:%d" % (l["auction_id"], n, int(time.time() * 1000)))
                self.log("buy", "收购玩家挂单 %s ×%d，单价 %d，花费 %d（拍卖号 %d，卖家 %s）"
                         % (self.item_name(iid), n, unit, cost, l["auction_id"], c["owner_name"]),
                         auction_id=l["auction_id"])
                done.append({"auction_id": l["auction_id"], "count": n, "cost": cost})
            else:
                self.log("error", "收购失败 拍卖号 %d：%s" % (l["auction_id"], why), level="warn")
                done.append({"auction_id": l["auction_id"], "error": why})
        return done

    # ---------------- 孤儿邮件 ----------------
    def orphan_preview(self):
        oc = self.cfg.get("orphan_cleanup", {})
        bid = int(self.buyer_cfg.get("buyer_id", 1999999999))
        postal, letters = self.db.orphan_rows(bid, int(oc.get("min_age_minutes", 0)) * 60)
        for p in postal:
            p["item_name"] = self.item_name(int(p["item_id"])) if p["item_id"] and p["item_id"] != "0" else ""
        return {"charac_no": bid, "postal": postal, "letters": letters,
                "enabled": bool(oc.get("enabled")), "dry_run": bool(oc.get("dry_run", True))}

    def orphan_cleanup_run(self, confirm_ids=None):
        oc = self.cfg.get("orphan_cleanup", {})
        prev = self.orphan_preview()
        bid = prev["charac_no"]
        if bid != int(self.buyer_cfg.get("buyer_id", 1999999999)) or self.buyer_cfg.get("_blocked"):
            raise PermissionError("收购角色 ID 异常，拒绝清理")
        pids = [int(p["postal_id"]) for p in prev["postal"]]
        lids = sorted({int(l["letter_id"]) for l in prev["letters"]})
        if not pids and not lids:
            return {"deleted": False, "postal": 0, "letters": 0, "msg": "没有需要清理的孤儿邮件"}
        if not oc.get("enabled") or oc.get("dry_run", True):
            self.log("orphan", "孤儿邮件清理（演练，未删除）：将删除 postal %d 条、letter %d 条" % (len(pids), len(lids)))
            return {"deleted": False, "postal": len(pids), "letters": len(lids), "msg": "演练模式：未删除任何数据"}
        for k in range(0, len(pids), 500):
            ch = pids[k:k + 500]
            self.db.write("DELETE FROM postal WHERE receive_charac_no=%d AND postal_id IN (%s)"
                          % (bid, ",".join(map(str, ch))), self.db.gdb)
        for k in range(0, len(lids), 500):
            ch = lids[k:k + 500]
            self.db.write("DELETE FROM letter WHERE charac_no=%d AND letter_id IN (%s)"
                          % (bid, ",".join(map(str, ch))), self.db.gdb)
        self.log("orphan", "孤儿邮件清理：已删除 postal %d 条、letter %d 条" % (len(pids), len(lids)))
        return {"deleted": True, "postal": len(pids), "letters": len(lids), "msg": "已删除"}

    # ---------------- 跨线程调用（供 Web 使用） ----------------
    def call(self, coro_fn, timeout=60):
        if not self.loop:
            raise RuntimeError("机器人尚未启动")
        fut = asyncio.run_coroutine_threadsafe(coro_fn(), self.loop)
        return fut.result(timeout)

    def status(self):
        c = self.client
        now = time.time()
        act = self.store.query("SELECT item_id, COUNT(*) n, SUM(count_now) q FROM bot_listings WHERE status='active' GROUP BY item_id")
        return {
            "now": now, "started_at": self.started_at, "paused": self.paused,
            "connected": c.connected, "ready": c.ready, "service_available": c.service_available,
            "connected_at": c.connected_at, "last_rx": c.last_rx, "last_tx": c.last_tx, "stats": c.stats,
            "regist_ack": c.regist_ack,
            "seller_enabled": bool(self.cfg.get("seller", {}).get("enabled", True)),
            "buyer_enabled": bool(self.buyer_cfg.get("enabled")), "buyer_blocked": bool(self.buyer_cfg.get("_blocked")),
            "orphan_enabled": bool(self.cfg.get("orphan_cleanup", {}).get("enabled")),
            "orphan_dry_run": bool(self.cfg.get("orphan_cleanup", {}).get("dry_run", True)),
            "last_scan": self.store.kv_get("last_scan"), "npc_name": self.store.kv_get("npc_name"),
            "seller_items": [{"item_id": i, "name": self.item_name(i), "base_price": v["base_price"],
                              "target_listings": int(v.get("target_listings", 3)),
                              "active": next((a["n"] for a in act if a["item_id"] == i), 0),
                              "active_qty": next((a["q"] for a in act if a["item_id"] == i), 0),
                              "disabled": self.disabled_items.get(i)} for i, v in self.seller_items.items()],
            "listings_last_hour": len([t for t in self.hourly if now - t < 3600]),
            "db_error": self.db.last_error,
        }
