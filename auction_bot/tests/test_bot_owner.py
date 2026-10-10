# -*- coding: utf-8 -*-
"""机器人卖家/买家 -114514（remake 拍卖服 DNF-MOD 2026-10-11）：挂单记录 owner_id、在架时长（退役时间）、到时取消轮换、旧模式 -1。"""
import asyncio
import os
import sys
import time
import unittest

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE)
from tests import mock_server                                  # noqa: E402
from tests import test_equip                                  # noqa: E402
from auction_bot.bot import BOT_OWNER, USER_EXPIRE_SEC        # noqa: E402


class TestBotOwner(unittest.TestCase):
    SHELF = test_equip.TestEquipShelf.SHELF

    def _bot(self, port, owner=None):
        if owner is not None:
            b = test_equip.TestEquipShelf._bot(self, port)
            b.cfg["bot_owner"] = {"id": owner}
            b.reload_config(b.cfg)
            return b
        return test_equip.TestEquipShelf._bot(self, port)

    def test_identity_and_life(self):
        b = self._bot(1)
        self.assertEqual((b.seller_id, b.vbuyer_id, b.vbuyer_name, b.legacy_owner), (BOT_OWNER, BOT_OWNER, "拍卖行", False))
        self.assertEqual(b.listing_life(5, "eq:shelf"), 7200)
        self.assertEqual(b.listing_life(5, None), 7200)
        self.assertEqual(b.listing_life(5, "manual"), USER_EXPIRE_SEC)
        lives = [b.listing_life(a, "floor") / 3600 for a in range(1000, 3000)]
        self.assertTrue(12 <= min(lives) and max(lives) <= 36)
        self.assertAlmostEqual(sum(lives) / len(lives), 24, delta=0.5)        # 均匀错开，平均 24 小时
        b.record_listing(77, 3274, 5, 100, 500, "floor")
        r = b.store.query("SELECT * FROM bot_listings WHERE auction_id=77")[0]
        self.assertEqual(r["owner_id"], BOT_OWNER)
        legacy = self._bot(1, owner=-1)
        self.assertEqual((legacy.seller_id, legacy.vbuyer_id, legacy.legacy_owner), (-1, 1999999999, True))
        self.assertEqual(legacy.listing_life(5, "floor"), 7200)

    def test_retire_against_mock(self):
        asyncio.run(self._run())

    async def _run(self):
        from auction_bot.client import AuctionClient
        mock_server.KNOWN_ITEMS[3274] = True
        m, srv, port = await mock_server.start()
        b = self._bot(port)
        b.client = AuctionClient("127.0.0.1", port, lambda *a, **k: None, 3, 0)
        await b.client.connect()
        try:
            r = await b.client.regist_item(b.seller_id, "", 3274, 5, 100)
            self.assertTrue(r["ok"])
            aid = r["auction_id"]
            self.assertEqual(m.listings[aid]["owner_name"], "拍卖行".encode())
            b.record_listing(aid, 3274, 5, 100, 500, None)
            self.assertEqual(await b.retire_tick(), 0)                       # 还没到退役时间
            b.store.execute("UPDATE bot_listings SET expire_at=? WHERE auction_id=?", (time.time() - 1, aid))
            b.runner.expired_pending.clear()
            self.assertEqual(await b.retire_tick(), 1)
            self.assertNotIn(aid, m.listings)
            self.assertEqual(m.history[-1], ("cancel", aid, BOT_OWNER, 5))
            self.assertEqual(b.store.query("SELECT status FROM bot_listings WHERE auction_id=?", (aid,))[0]["status"], "expired")
            if 3274 in b.runner.items:
                self.assertEqual(b.runner.expired_pending.get(3274), [5])      # 引擎照旧补挂（与 -1 到期相同）
            # 机器人可以买自己的挂单（拍卖服 MOD），玩家不能买自己的
            r = await b.client.regist_item(b.seller_id, "", 3274, 2, 100)
            rr = await b.client.buy_apiece(BOT_OWNER, r["auction_id"], 100, 2)
            self.assertTrue(rr["ok"])
            r = await b.client.regist_item(7, "玩家", 3274, 2, 100)
            rr = await b.client.buy_apiece(7, r["auction_id"], 100, 2)
            self.assertFalse(rr["ok"])
            # 旧 -1 挂单不取消（由拍卖服 2 小时回收）
            b.store.execute("INSERT INTO bot_listings(auction_id,item_id,count0,count_now,unit_price,instant_price,listed_at,"
                            "expire_at,status,last_seen) VALUES(999,3274,1,1,1,1,0,1,'active',0)")
            self.assertEqual(await b.retire_tick(), 0)
        finally:
            await b.client.close()
            srv.close()


if __name__ == "__main__":
    unittest.main()
