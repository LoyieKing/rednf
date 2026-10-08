# -*- coding: utf-8 -*-
"""经济模型引擎：与 sim.py 的价格校准一致性 + 接入 mock 拍卖服后的执行/护栏。"""
import asyncio
import collections
import json
import os
import random
import statistics
import sys
import tempfile
import time
import unittest

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE)
from auction_bot.engine import Engine           # noqa: E402
from auction_bot.runner import load_excluded    # noqa: E402
from tests import mock_server                   # noqa: E402

MODEL = json.load(open(os.path.join(BASE, "ref/model_config.json"), encoding="utf-8"))
EXCL = load_excluded(os.path.join(BASE, "ref/excluded_items.json"))


class TestEngineOffline(unittest.TestCase):
    def test_excluded_not_in_model(self):
        self.assertIn(3037, EXCL)
        self.assertIn(3036, EXCL)
        self.assertIn(1047, EXCL)
        self.assertEqual(len([k for k in EXCL if k in (3037, 3033, 3034, 3035, 3036, 3262, 3042, 3038, 3039, 3040, 3041)]), 11)
        for k in MODEL["items"]:
            self.assertNotIn(int(k), EXCL)

    def test_prices_track_reference(self):
        """3 天、10 分钟一步、自建挂单簿：日 VWAP 中位数偏差应与 sim.py 校准同量级。"""
        items = {int(k): v for k, v in MODEL["items"].items()}
        eng = Engine(MODEL, items, {"volume_scale": 1.0}, rng=random.Random(3))
        t0 = time.mktime((2026, 10, 5, 0, 0, 0, 0, 0, -1))
        book = {i: [] for i in items}
        nid = 1
        trades = collections.defaultdict(list)
        for s in range(3 * 144):
            now = t0 + s * 600
            for i in items:
                book[i] = [a for a in book[i] if now - a["born"] < 7200 and a["qty"] > 0]
            plan = eng.step(10 / 60, {i: [dict(a) for a in b] for i, b in book.items()}, now=now)
            ids = []
            for n in plan["new"]:
                book[n["item_id"]].append({"auction_id": nid, "price": n["price"], "qty": n["qty"], "owner_id": -1,
                                           "seller": n["seller"], "buyable": True, "born": now})
                ids.append(nid)
                nid += 1
            for f in plan["fills"]:
                aid = f["auction_id"] or ids[f["new_idx"]]
                for a in book[f["item_id"]]:
                    if a["auction_id"] == aid:
                        a["qty"] -= f["qty"]
                        self.assertGreaterEqual(a["qty"], 0)
                if s >= 144:
                    trades[f["item_id"]].append((f["unit_price"], f["qty"]))
        errs = []
        for i, p in items.items():
            tr = trades[i]
            if tr:
                vw = sum(a * b for a, b in tr) / sum(b for _, b in tr)
                errs.append(abs(vw / p["ref"] - 1))
        self.assertGreater(len(errs), 45)
        self.assertLess(statistics.median(errs), 0.06)
        # 神秘商店道具（锚定商店基价）：VWAP 应落在评估带内
        for i in (3187, 3221, 3229, 3300):
            tr = trades[i]
            vw = sum(a * b for a, b in tr) / sum(b for _, b in tr)
            lo, hi = items[i]["band"]
            self.assertTrue(lo <= vw <= hi, (i, vw, lo, hi))

    def test_secret_shop_and_anchor_reset(self):
        """旧状态（无 ref 字段、last=旧史料价）在锚点改变后重置；市价高于商店价时出现神秘商店转卖挂单。"""
        items = {3229: dict(MODEL["items"]["3229"])}
        self.assertEqual(items[3229]["ref"], 500.0)
        old = {"3229": {"stock": 20000.0, "x": 0.0, "last": 1500.0}}
        eng = Engine(MODEL, items, {"volume_scale": 1.0}, state=old, rng=random.Random(5))
        self.assertEqual(eng.state["3229"]["last"], 500.0)
        self.assertEqual(eng.state["3229"]["ref"], 500.0)
        # 人为把成交价抬高到商店价的 1.5 倍 → 应有玩家去神秘商店买并转卖
        items[3229]["secret_shop"] = dict(items[3229]["secret_shop"], p_offer_per_run=0.05)
        sellers = collections.Counter()
        t0 = time.mktime((2026, 10, 5, 20, 0, 0, 0, 0, -1))
        for s in range(50):
            eng.state["3229"]["last"] = 750.0
            plan = eng.step(10 / 60, {3229: []}, now=t0 + s * 600)
            for n in plan["new"]:
                sellers[n["seller"]] += 1
                if n["seller"] == "secret_shop_resale":
                    self.assertTrue(450 * 1.05 - 1 <= n["price"] <= 550 * 1.15 + 1, n)
        self.assertGreater(sellers["secret_shop_resale"], 0)

    def test_user_priority(self):
        items = {3303: MODEL["items"]["3303"]}
        eng = Engine(MODEL, items, {"volume_scale": 30.0}, rng=random.Random(1))
        book = {3303: [{"auction_id": 9, "price": 500, "qty": 5, "owner_id": 0, "seller": "user", "buyable": True},
                       {"auction_id": 8, "price": 400, "qty": 50, "owner_id": -1, "seller": "sys", "buyable": True}]}
        plan = eng.step(10 / 60, book, now=time.mktime((2026, 10, 8, 15, 0, 0, 0, 0, -1)))
        fills = [f for f in plan["fills"] if f["auction_id"] in (8, 9)]
        self.assertTrue(any(f["auction_id"] == 9 for f in fills))
        # 玩家挂单（更贵）也先于系统挂单成交
        first = plan["fills"][0]
        self.assertEqual(first["auction_id"], 9)


class TestRunnerMock(unittest.TestCase):
    def test_step_against_mock(self):
        asyncio.run(self._run())

    async def _run(self):
        from auction_bot.bot import Bot
        from auction_bot.client import AuctionClient
        for k in list(MODEL["items"]) + ["3037"]:
            mock_server.KNOWN_ITEMS[int(k)] = True
        m, srv, port = await mock_server.start()
        tmp = tempfile.mkdtemp()
        cfg = {"data_dir": tmp, "auction_server": {"host": "127.0.0.1", "port": port, "min_interval": 0},
               "game_db": {"mysql_bin": "/bin/false"},
               "market": {"watch_reference_items": False},
               "seller": {"enabled": False, "items": [{"item_id": 3037, "base_price": 45}]},
               "engine": {"enabled": True, "model_config": os.path.join(BASE, "ref/model_config.json"),
                          "excluded_items": os.path.join(BASE, "ref/excluded_items.json"),
                          "items_whitelist": [3303, 3218, 3037], "volume_scale": 40.0,
                          "virtual_fill": {"max_per_step": 1000, "max_per_day": 100000},
                          "user_fill": {"enabled": True, "max_gold_per_day": 3000, "max_gold_per_buy": 2000, "probability": 1.0}},
               "buyer": {"enabled": False}}
        bot = Bot(cfg, BASE, {})
        self.assertNotIn(3037, bot.seller_items)
        self.assertNotIn(3037, bot.runner.items)
        self.assertEqual(set(bot.runner.items), {3303, 3218})
        bot.loop = asyncio.get_running_loop()
        bot.client = AuctionClient("127.0.0.1", port, lambda *a, **k: None, 3, 0)
        await bot.client.connect()
        # 玩家挂单：3303 低价（应被优先买，受金币上限）、3037 晶块（永不买）
        m.listings[1] = {"auction_id": 1, "owner": 7, "owner_name": "玩家".encode(), "price": -1, "instant_price": 900 * 10,
                         "unit_price": 900, "item_id": 3303, "count": 10, "expire": 0}
        m.listings[2] = {"auction_id": 2, "owner": 7, "owner_name": "玩家".encode(), "price": -1, "instant_price": 10 * 20,
                         "unit_price": 10, "item_id": 3037, "count": 20, "expire": 0}
        m.listings[3] = {"auction_id": 3, "owner": 7, "owner_name": "玩家".encode(), "price": -1, "instant_price": 2000 * 5,
                         "unit_price": 2000, "item_id": 3303, "count": 5, "expire": 0}   # 高于价格带上限×1.25：护栏不买
        bot.watch_ids.add(3037)
        bot.runner.engine.state["3303"]["x"] = 0.0
        res1 = await bot.runner.step()
        res2 = await bot.runner.step()
        self.assertGreater(res1["new_done"] + res2["new_done"], 0)
        self.assertGreater(res1["vf_done"] + res2["vf_done"], 0)
        self.assertEqual(m.listings[2]["count"], 20)                 # 晶块从未被买
        self.assertEqual(m.listings[3]["count"], 5)                  # 价格带护栏
        spent = sum(d["total"] for d in bot.store.query("SELECT total FROM deals WHERE side='bot_buy_human'"))
        self.assertLessEqual(spent, 3000)                            # 每日金币上限
        self.assertGreater(spent, 0)
        # 机器人自己的挂单数量与拍卖服一致（虚拟成交已同步）
        for r in bot.store.query("SELECT * FROM bot_listings WHERE status='active'"):
            self.assertEqual(m.listings[r["auction_id"]]["count"], r["count_now"])
        for r in bot.store.query("SELECT * FROM bot_listings WHERE status='sold'"):
            self.assertNotIn(r["auction_id"], m.listings)
        buyers = {h[2] for h in m.history}
        self.assertEqual(buyers, {1999999999})
        # 扫描对账：不应把虚拟成交误判为“玩家买系统货”
        await bot.scan_market()
        self.assertEqual(bot.store.query("SELECT COUNT(*) n FROM deals WHERE side='human_buy_bot'")[0]["n"], 0)
        st = bot.runner.status()
        self.assertEqual(len(st["items"]), 2)
        await bot.client.close()
        srv.close()
        self.assertEqual(m.violations, [])


if __name__ == "__main__":
    unittest.main(verbosity=2)
