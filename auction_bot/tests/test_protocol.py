# -*- coding: utf-8 -*-
"""协议编解码单测（偏移量以 remake 源码 AuctionPacket.h / AuctionItem.h 为准）+ mock 拍卖服端到端测试。"""
import asyncio
import os
import struct
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from auction_bot import protocol as P          # noqa: E402
from tests import mock_server                  # noqa: E402


class TestCodec(unittest.TestCase):
    def test_header(self):
        self.assertEqual(P.HEADER_SIZE, 18)
        h = P.header(0, 3, 0xc5)
        self.assertEqual(h, bytes([0, 3]) + struct.pack("<IIII", 0xc5, 0, 0, 0))

    def test_fixed_sizes_match_source(self):
        # PACKET_CTOR_BODY(0, msg, size) in AuctionPacket.h
        exp = {0: 0x16, 2: 0x3d, 3: 0xc5, 4: 0x22, 5: 0x33, 6: 0x81, 8: 0x1a, 13: 0x12, 14: 0x2a}
        for k, v in exp.items():
            self.assertEqual(P.GA_SIZES[k], v, k)
        self.assertEqual(len(P.build_regist()), 0x16)
        self.assertEqual(len(P.build_check_ready()), 0x12)
        self.assertEqual(len(P.build_ask_registed_item_num(1, -1, 3037)), 0x3d)
        self.assertEqual(len(P.build_regist_item(1, -1, "", 3037, 10, -1, 450, 45)), 0xc5)
        self.assertEqual(len(P.build_bidding(1, 1999999999, "收购商", 100, 5)), 0x33)
        self.assertEqual(len(P.build_buy_item_apiece(1, 1999999999, 5000, 1, 1)), 0x2a)
        self.assertEqual(len(P.build_search_by_itemkey(1, 0, [3037])), 0x81)
        self.assertEqual(len(P.build_my_registed_item_info(1, -1)), 0x1a)
        self.assertEqual(len(P.build_regist_cancel(1, -1, 5)), 0x22)

    def test_item_info_layout(self):
        self.assertEqual(P.DNF_ITEM_INFO_SIZE, 53)
        b = P.pack_item_info(3037, add_info=123, endurance=7)
        self.assertEqual(len(b), 53)
        self.assertEqual(struct.unpack_from("<I", b, 1)[0], 3037)
        self.assertEqual(struct.unpack_from("<i", b, 6)[0], 123)
        self.assertEqual(struct.unpack_from("<H", b, 10)[0], 7)
        it = P.unpack_item_info(b)
        self.assertEqual((it["item_id"], it["add_info"]), (3037, 123))

    def test_regist_item_offsets(self):
        b = P.build_regist_item(0x11223344, -1, "测试", 3037, 77, -1, 77 * 45, 45)
        self.assertEqual(struct.unpack_from("<ii", b, 18), (0x11223344, -1))
        self.assertEqual(b[26:39].split(b"\0")[0], "测试".encode())
        self.assertEqual(b[39], 0)                                         # user_type
        self.assertEqual(struct.unpack_from("<ii", b, 40), (-1, 77 * 45))  # price, instant
        self.assertEqual(struct.unpack_from("<I", b, 49)[0], 3037)         # item_info@48 + item_id@1
        self.assertEqual(struct.unpack_from("<i", b, 54)[0], 77)           # add_info
        self.assertEqual(struct.unpack_from("<i", b, 149)[0], 45)          # unit_price
        self.assertEqual(b[153:165], b"\0" * 12)                           # ROI
        self.assertEqual(b[165:197], b"\0" * 32)                           # owner_nexon_id

    def test_cstr_truncation_keeps_utf8_valid(self):
        s = P._cstr("拍卖行收购测试", 13)
        self.assertEqual(len(s), 13)
        self.assertEqual(s[-1], 0)
        s.split(b"\0")[0].decode("utf-8")

    def test_decode_truncated_utf8_name(self):
        # 真服实测：「测试角色」在 owner_name[13] 中被截成 e6b58b e8af95 e8a792 e889 00
        self.assertEqual(P.decode_name(bytes.fromhex("e6b58be8af95e8a792e8890000")), "测试角")
        self.assertEqual(P.decode_name("로저레빈".encode() + b"\0"), "로저레빈")

    def test_apiece_and_bidding_offsets(self):
        b = P.build_buy_item_apiece(9, 1999999999, 5000, 1, 1)
        self.assertEqual(struct.unpack_from("<iiiQi", b, 18), (9, 1999999999, 5000, 1, 1))
        b = P.build_bidding(9, 1999999999, "收购商", 150000, 1)
        self.assertEqual(struct.unpack_from("<iQ", b, 39), (150000, 1))

    def test_search_validation(self):
        with self.assertRaises(P.ProtocolError):
            P.build_search_by_itemkey(1, 0, list(range(21)))
        with self.assertRaises(P.ProtocolError):
            P.build_search_by_itemkey(1, 0, [1], upgrade_end=32)
        with self.assertRaises(P.ProtocolError):
            P.build_search_by_itemkey(1, 0, [1], refine_end=0x80)
        b = P.build_search_by_itemkey(5, 0, [3037, 3036], start_index=60)
        self.assertEqual(struct.unpack_from("<IBBBH", b, 26), (60, 0, 31, 2, 0))
        self.assertEqual(struct.unpack_from("<BB", b, 47), (0, 0x7f))
        self.assertEqual(struct.unpack_from("<2I", b, 49), (3037, 3036))

    def test_parse_item_list(self):
        l = {"auction_id": 1, "price": -1, "instant_price": 150000, "unit_price": 5000, "item_id": 1047, "count": 30,
             "owner_name": "测试角色".encode()}
        body = struct.pack("<iiBIH", 7, 0, 0, 1, 1) + mock_server.pack_auction_item_info(l)
        pkt = mock_server.ag(P.AG_ITEM_LIST, body)
        self.assertEqual(len(pkt), 33 + 137)
        m = P.parse_ag(pkt)
        self.assertEqual(m["type"], "ITEM_LIST")
        it = m["items"][0]
        self.assertEqual((it["auction_id"], it["item_id"], it["count"], it["unit_price"], it["owner_name"]),
                         (1, 1047, 30, 5000, "测试角色"))

    def test_frame_reader(self):
        fr = P.FrameReader()
        a = mock_server.ag(P.AG_REGIST_ACK, b"")
        c = mock_server.ag(P.AG_CHECK_AUCTION_READY, b"\1")
        out = fr.feed(a + c[:5])
        self.assertEqual(len(out), 1)
        out = fr.feed(c[5:])
        self.assertEqual(out, [c])
        with self.assertRaises(P.ProtocolError):
            P.FrameReader().feed(struct.pack(P.HEADER_FMT, 1, 0, 0, 0, 0, 0))


class TestAgainstMock(unittest.TestCase):
    def test_end_to_end(self):
        asyncio.run(self._e2e())

    async def _e2e(self):
        from auction_bot.client import AuctionClient
        from auction_bot.bot import Bot
        m, srv, port = await mock_server.start()
        logs = []
        tmp = tempfile.mkdtemp()
        cfg = {"data_dir": tmp, "auction_server": {"host": "127.0.0.1", "port": port, "min_interval": 0},
               "game_db": {"mysql_bin": "/bin/false"},
               "market": {"watch_reference_items": False, "extra_watch_items": [1047]},
               "seller": {"enabled": True, "max_new_listings_per_cycle": 10,
                          "items": [{"item_id": 3037, "base_price": 45, "target_listings": 2, "count_min": 5, "count_max": 9},
                                    {"item_id": 99999, "base_price": 10, "target_listings": 1}]},
               "buyer": {"enabled": False}}
        items = {3037: {"name": "无色小晶块", "kind": "stackable", "stack_limit": 1000},
                 99999: {"name": "不存在", "kind": "stackable"},
                 1047: {"name": "命运硬币", "kind": "stackable"}}
        bot = Bot(cfg, tmp, items)
        bot.loop = asyncio.get_running_loop()
        bot.client = AuctionClient("127.0.0.1", port, lambda *a, **k: logs.append(a), 3, 0)
        await bot.client.connect()
        self.assertTrue(bot.client.ready)
        # 人类挂单
        m.listings[1] = {"auction_id": 1, "owner": 1, "owner_name": "测试角色".encode(), "price": -1,
                         "instant_price": 150000, "unit_price": 5000, "item_id": 1047, "count": 30, "expire": 0}
        self.assertEqual(await bot.client.probe_item(99999), -2)
        await bot.restock()
        mine = [l for l in m.listings.values() if l["owner"] == -1]
        self.assertEqual(len(mine), 2)
        for l in mine:
            self.assertEqual(l["price"], -1)
            self.assertEqual(l["instant_price"], l["unit_price"] * l["count"])
            self.assertTrue(5 <= l["count"] <= 9)
        self.assertIn(99999, bot.disabled_items)
        await bot.scan_market()
        self.assertEqual(bot.store.kv_get("npc_name"), "拍卖行")
        # 玩家买走一部分
        aid = mine[0]["auction_id"]
        m.listings[aid]["count"] -= 2
        await bot.scan_market()
        d = bot.store.query("SELECT * FROM deals WHERE auction_id=?", (aid,))
        self.assertEqual(d[0]["count"], 2)
        # 受控收购：1 个
        bot.buyer_cfg = {"buyer_id": 1999999999, "buyer_name": "收购商"}
        bot.db.auction_main = lambda where: [{"auction_id": 1, "owner_id": 1, "owner_name": "测试角色", "item_id": 1047}]
        r = await bot.auto_buy(force_auction_id=1, force_count=1)
        self.assertEqual(r[0]["count"], 1)
        self.assertEqual(m.listings[1]["count"], 29)
        self.assertEqual(m.history[-1], ("apiece", 1, 1999999999, 1))
        # 检索分页（> 60 条）
        for i in range(70):
            m.listings[1000 + i] = dict(m.listings[1], auction_id=1000 + i)
        ls = await bot.client.search_items([1047])
        self.assertEqual(len(ls), 71)
        # 非法检索：本地拦截
        with self.assertRaises(P.ProtocolError):
            await bot.client.request(lambda i: P.build_search_by_itemkey(i, 0, list(range(1, 30))), ["ITEM_LIST"])
        await bot.client.close()
        srv.close()
        self.assertEqual(m.violations, [])
        regs = [r for r in m.received if r[1] == 0 and r[0] == 0]
        self.assertEqual(len(regs), 1)


if __name__ == "__main__":
    unittest.main(verbosity=2)
