# -*- coding: utf-8 -*-
"""保底挂单（engine.floor）：每个范围内道具保持 ≥ min_visible 张可见系统挂单；到期前补挂；对材料引擎不可见；极低虚拟购买。"""
import asyncio
import math
import os
import sys
import time
import unittest

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE)
from tests import mock_server                                  # noqa: E402
from tests import test_equip                                  # noqa: E402

FLOOR = {"enabled": True, "min_visible": 3, "scope": {"all": False, "tiers": ["EQ_A"], "items": [3274]},
         "interval_sec": 20, "lead_minutes": 10, "ramp_minutes": 0.01, "max_new_per_min": 1000,
         "equipment": {"target_sell_through": 0.004, "min_shelf_minutes": 30}, "stackable": {"buy_hazard_per_h": 0}}


class TestFloor(unittest.TestCase):
    SHELF = test_equip.TestEquipShelf.SHELF

    def _bot(self, port):
        b = test_equip.TestEquipShelf._bot(self, port)
        b.cfg["engine"]["floor"] = {"enabled": False}
        return b

    def test_hazard_and_scope(self):
        bot = self._bot(1)
        fl = bot.runner.floor
        self.assertEqual(fl.scope(), {})
        self.assertFalse(fl.covers_tier("EQ_A"))
        bot.cfg["engine"]["floor"] = dict(FLOOR)
        sc = fl.scope()
        self.assertEqual(sc[3274], "stack")
        self.assertEqual({i for i, k in sc.items() if k == "eq"}, {i for i, c in bot.runner.equip.catalog.items() if c["tier"] == "EQ_A"})
        self.assertTrue(fl.covers_tier("EQ_A"))
        self.assertFalse(fl.covers_tier("EQ_HI"))
        # 0.4% 在 (120-30) 分钟内卖出
        h = fl.eq_hazard("EQ_A")
        self.assertAlmostEqual(1 - math.exp(-h * 1.5), 0.004)
        # 限速：总量 / ramp，不超过 max_new_per_min
        bot.cfg["engine"]["floor"] = dict(FLOOR, ramp_minutes=110, max_new_per_min=45, min_visible=10)
        self.assertAlmostEqual(fl.rate(584), 45)                     # 护栏 max_new_per_min 生效
        self.assertAlmostEqual(fl.rate(20), 1.15 * 200 / 110)

    def test_floor_against_mock(self):
        asyncio.run(self._run())

    async def _run(self):
        from auction_bot.client import AuctionClient
        for k in (10478, 20066, 31119, 20203, 2200110, 3274):
            mock_server.KNOWN_ITEMS[k] = k == 3274
        m, srv, port = await mock_server.start()
        bot = self._bot(port)
        bot.loop = asyncio.get_running_loop()
        bot.client = AuctionClient("127.0.0.1", port, lambda *a, **k: None, 3, 0)
        await bot.client.connect()
        fl = bot.runner.floor
        self.assertEqual((await fl.tick())["listed"], 0)              # 默认关闭
        bot.cfg["engine"]["floor"] = dict(FLOOR)
        sc = fl.scope()

        def sys_of(iid):
            return [l for l in m.listings.values() if l["owner"] == -1 and l["item_id"] == iid]
        r = await fl.tick()
        self.assertEqual(r["listed"], 3 * len(sc), (r, fl.tokens))
        for iid in sc:
            self.assertEqual(len(sys_of(iid)), 3)
        notes = {x["note"] for x in bot.store.query("SELECT note FROM bot_listings WHERE status='active'")}
        self.assertEqual(notes, {"floor", "floor:eq"})
        for l in sys_of(3274):
            self.assertEqual(l["price"], -1)
            lo, hi = bot.runner.band(3274)
            self.assertTrue(lo <= l["unit_price"] <= hi)
        eq = bot.runner.equip
        for iid, k in sc.items():
            if k == "eq":
                for l in sys_of(iid):
                    lo, hi = eq.band(iid)
                    self.assertTrue(lo <= l["instant_price"] <= hi)
        self.assertEqual((await fl.tick())["listed"], 0)              # 已满，不超挂
        # 到期前 lead_minutes：先补一张新的，旧的还在 → 不出现空档
        aid = sys_of(3274)[0]["auction_id"]
        bot.store.execute("UPDATE bot_listings SET expire_at=? WHERE auction_id=?", (time.time() + 300, aid))
        r = await fl.tick()
        self.assertEqual(r["listed"], 1)
        self.assertEqual(len(sys_of(3274)), 4)
        # 材料引擎看不到 floor 挂单；shelf 不再给 floor 覆盖的层级补挂
        seen = {}
        orig = bot.runner.engine.step

        def spy(dt_h, book, expired=None, now=None):
            seen.update(book)
            return orig(dt_h, book, expired, now)
        bot.runner.engine.step = spy
        await bot.runner.step()
        floor_ids = {x["auction_id"] for x in bot.store.query("SELECT auction_id FROM bot_listings WHERE note LIKE 'floor%'")}
        self.assertTrue(all(a["auction_id"] not in floor_ids for a in seen.get(3274, [])))
        self.assertEqual(bot.store.query("SELECT COUNT(*) n FROM bot_listings WHERE note='eq:shelf'")[0]["n"], 0)
        # floor 挂单不写 listing_obs
        n = bot.store.query("SELECT COUNT(*) n FROM listing_obs WHERE auction_id IN (%s)" % ",".join(map(str, floor_ids)))[0]["n"]
        self.assertEqual(n, 0)
        # 虚拟买家：装备 floor 挂单挂满 30 分钟后按风险率买；材料 floor 挂单不买
        bot.cfg["engine"]["floor"]["equipment"] = {"buy_hazard_per_h": 1e6, "min_shelf_minutes": 30}
        r = await fl.tick()
        self.assertEqual(r["sold"], 0)                               # 不满 30 分钟
        bot.store.execute("UPDATE bot_listings SET listed_at=listed_at-1860 WHERE note LIKE 'floor%'")
        r = await fl.tick()
        n_eq = sum(1 for k in sc.values() if k == "eq") * 3
        self.assertEqual(r["sold"], n_eq)
        self.assertTrue(all(h[2] == 1999999999 for h in m.history))
        self.assertTrue(all(m.sold[h[1]]["item_id"] != 3274 for h in m.history))
        # 到期的 floor 挂单不交给引擎补挂
        bot.store.execute("UPDATE bot_listings SET expire_at=? WHERE note='floor'", (time.time() + 30,))
        await bot.scan_market()
        self.assertNotIn(3274, bot.runner.expired_pending)
        st = bot.runner.status()["floor"]
        self.assertTrue(st["enabled"])
        self.assertEqual(st["items"], len(sc))
        await bot.client.close()
        srv.close()
        self.assertEqual(m.violations, [])


if __name__ == "__main__":
    unittest.main(verbosity=2)
