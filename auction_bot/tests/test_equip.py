# -*- coding: utf-8 -*-
"""装备挂单（v4.1 equipment 段）：格式、过滤、价格、guid、端到端（mock 拍卖服）。"""
import asyncio
import os
import struct
import sys
import tempfile
import unittest

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE)
from auction_bot import protocol as P          # noqa: E402
from tests import mock_server                   # noqa: E402

TIERS = {
    "EQ_A": {"name": "便宜层", "ref": 30000.0, "band": [24000.0, 37500.0], "c": 27000.0, "v": 45000.0, "faucet_per_h": 0.5,
             "sink_rate_per_h": 0.01, "epsilon": 1.5, "kappa_per_day": 0.17, "sigma_per_day": 0.04, "ou_sd": 0.07,
             "activity": 0.3, "target_listings": 4, "lot_qty": {"median": 1, "sigma": 0, "min": 1, "max": 1},
             "consumer_qty": {"median": 1, "sigma": 0, "min": 1, "max": 1}, "price_mult": 1.0},
    "EQ_HI": {"name": "4 亿层", "ref": 190000000.0, "band": [152000000.0, 237500000.0], "c": 170000000.0, "v": 290000000.0,
              "faucet_per_h": 0.08, "sink_rate_per_h": 0.003, "epsilon": 1.8, "kappa_per_day": 0.14, "sigma_per_day": 0.04,
              "ou_sd": 0.07, "activity": 0.1, "target_listings": 2, "lot_qty": {"median": 1, "sigma": 0, "min": 1, "max": 1},
              "consumer_qty": {"median": 1, "sigma": 0, "min": 1, "max": 1}, "price_mult": 1.0},
}


def item(tier, ref, band, ro=False):
    return {"tier": tier, "ref": ref, "band": band, "supply_weight": 0.5, "random_option_in_pvf": ro, "name": "x",
            "listing": {"upgrade": 0, "seal": 1, "endurance": 33, "random_option": None, "separate_upgrade": 0}}


ITEMS = {"10478": item("A", 30000.0, [30000.0, 350000.0]),
         "20203": item("A", 30000.0, [24000.0, 37500.0], ro=True),          # 假紫（csv 标记）
         "2200110": item("A", 30000.0, [24000.0, 37500.0]),                # csv 没标、但 PVF 有 [random option]
         "20066": item("A", 30000.0, [24000.0, 37500.0]),                  # 首饰：PVF 无耐久
         "31119": item("HI", 400000000.0, [320000000.0, 500000000.0]),
         "99999": item("A", 30000.0, [24000.0, 37500.0])}                  # 不能登记拍卖的 attach type
META = {10478: {"name": "爆麟夹克", "kind": "equipment", "attach": "[sealing]", "durability": 33, "random_option": False, "path": "equipment/x.equ"},
        20203: {"name": "银波石项链", "kind": "equipment", "attach": "[sealing]", "durability": None, "random_option": True, "path": "equipment/y.equ",
                "rarity": 2, "level": 50, "item_group": "amulet", "ro_grade": 1},
        2200110: {"name": "狂狮左轮", "kind": "equipment", "attach": "[sealing]", "durability": 35, "random_option": True, "path": "equipment/z.equ",
                  "rarity": 2, "level": 50, "item_group": "amulet", "ro_grade": 1},
        20066: {"name": "项链", "kind": "equipment", "attach": "[free]", "durability": None, "random_option": False, "path": "equipment/n.equ"},
        31119: {"name": "冰焰左轮枪", "kind": "equipment", "attach": "[sealing]", "durability": 40, "random_option": False, "path": "equipment/w.equ"},
        99999: {"name": "账号绑定", "kind": "equipment", "attach": "[account]", "durability": 30, "random_option": False, "path": "equipment/v.equ"},
        3274: {"name": "扳手", "kind": "stackable", "stack_limit": 1000}}


class TestEquipFormat(unittest.TestCase):
    def test_item_info_layout(self):
        g = P.make_item_guid(3011, 1789805019, 60001)
        b = P.equipment_item_info(10478, 1, 33, 1403859482, g)
        self.assertEqual(len(b), 53)
        seal, iid, attr, ai, endu, ext = struct.unpack_from("<BIBiHI", b, 0)
        self.assertEqual((seal, iid, attr, ai, endu, ext), (1, 10478, 0, 1403859482, 33, 0))
        self.assertEqual(b[16:19], b"\0\0\0")                    # 无增幅
        self.assertEqual(b[19:29], g)
        self.assertEqual(struct.unpack_from("<iiH", b, 19), (3011, 1789805019, 60001))
        self.assertEqual(b[29:53], b"\0" * 24)                   # 随机属性/分离强化/保留 全 0
        u = P.unpack_item_info(b)
        self.assertTrue(u["random_empty"])
        self.assertEqual(u["guid"], g.hex())
        with self.assertRaises(P.ProtocolError):
            P.equipment_item_info(1, 1, 1, -1, g)

    def test_regist_packet(self):
        info = P.equipment_item_info(31119, 1, 40, 5, P.make_item_guid(3011, 1, 60000))
        pkt = P.build_regist_item(0, -1, "", 31119, 1, -1, 400000000, 400000000, item_info=info)
        self.assertEqual(len(pkt), P.GA_SIZES[P.GA_REGIST_ITEM])
        self.assertEqual(struct.unpack_from("<ii", pkt, 40), (-1, 400000000))
        self.assertEqual(pkt[48:101], info)
        self.assertEqual(pkt[139:149], b"\0" * 10)               # 外层 guid：ORIG 游戏端从不填
        self.assertEqual(struct.unpack_from("<i", pkt, 149)[0], 400000000)
        with self.assertRaises(P.ProtocolError):
            P.build_regist_item(0, -1, "", 31119, 1, -1, 2 ** 31, 2 ** 31, item_info=info)


class TestEquipRunner(unittest.TestCase):
    def _bot(self, port, eqcfg):
        from auction_bot.bot import Bot
        import json
        tmp = tempfile.mkdtemp()
        model = json.load(open(os.path.join(BASE, "ref/model_config.json"), encoding="utf-8"))
        model["equipment"] = {"tiers": TIERS, "items": ITEMS}
        mp = os.path.join(tmp, "m.json")
        json.dump(model, open(mp, "w", encoding="utf-8"), ensure_ascii=False)
        cfg = {"data_dir": tmp, "auction_server": {"host": "127.0.0.1", "port": port, "min_interval": 0},
               "game_db": {"mysql_bin": "/bin/false"}, "market": {"watch_reference_items": False},
               "seller": {"enabled": False, "items": []},
               "engine": {"enabled": True, "model_config": mp,
                          "excluded_items": os.path.join(BASE, "ref/excluded_items.json"),
                          "items_whitelist": [3274], "volume_scale": 0.0, "equipment": eqcfg},
               "buyer": {"enabled": False}}
        return Bot(cfg, BASE, dict(META))

    def test_catalog_and_prices(self):
        bot = self._bot(1, {"enabled": True, "format_verified": False})
        eq = bot.runner.equip
        self.assertEqual(set(eq.catalog), {10478, 20066, 31119})
        self.assertIn("假紫", eq.skipped[20203])
        self.assertIn("不一致", eq.skipped[2200110])
        self.assertIn("attach", eq.skipped[99999])
        self.assertFalse(eq.listing_enabled)                       # 格式未验证 → 不挂
        self.assertEqual(eq.catalog[10478]["endurance"], 33)       # PVF 满耐久
        self.assertEqual(eq.catalog[20066]["endurance"], 0)        # 首饰无耐久
        self.assertEqual(eq.catalog[10478]["seal"], 1)             # [sealing] → 封装
        self.assertEqual(eq.catalog[20066]["seal"], 0)             # [free] 不封装（make_item 只给 attach 3 封装）
        # 价格：层级价 → 单件价，夹在单件价格带内；4 亿不超 int32
        self.assertEqual(eq.item_price(10478, 30000), 30000)
        self.assertEqual(eq.item_price(10478, 10), 30000)          # 夹到带下限
        p = eq.item_price(31119, 190000000)
        self.assertEqual(p, 400000000)
        self.assertLessEqual(eq.item_price(31119, 10 ** 12), 500000000)
        self.assertLess(eq.item_price(31119, 10 ** 12), P.INT32_MAX)
        # guid 唯一、seq 落在 60000..65535
        gs = [eq.new_guid()[1] for _ in range(3000)]
        self.assertEqual(len(set(gs)), len(gs))
        self.assertTrue(all(60000 <= g[2] <= 65535 and g[0] == 3011 for g in gs))
        # 数量：装备永远 1
        self.assertEqual(bot.count_of(10478, 1403859482), 1)
        self.assertEqual(bot.count_of(3274, 7), 7)

    def test_step_against_mock(self):
        asyncio.run(self._run())

    async def _run(self):
        from auction_bot.client import AuctionClient
        from auction_bot.runner import manual_preview, manual_list
        for k in (10478, 20066, 31119, 20203, 2200110, 3274):
            mock_server.KNOWN_ITEMS[k] = k == 3274
        m, srv, port = await mock_server.start()
        bot = self._bot(port, {"enabled": True, "format_verified": True, "max_listings_per_tier": 2, "volume_scale": 400.0,
                               "max_new_per_step": 10, "max_new_per_hour": 100,
                               "virtual_fill": {"max_per_step": 10, "max_per_day": 100},
                               "user_fill": {"enabled": False, "max_gold_per_day": 20000000}})
        bot.loop = asyncio.get_running_loop()
        bot.client = AuctionClient("127.0.0.1", port, lambda *a, **k: None, 3, 0)
        await bot.client.connect()
        eq = bot.runner.equip
        self.assertTrue(eq.listing_enabled)
        # 玩家挂单：一件 +0 便宜装备（收购关闭，不能买）、一件假紫（永远不在簿里）
        info = P.equipment_item_info(10478, 1, 33, 7, P.make_item_guid(3011, 1, 5))
        m.listings[1] = {"auction_id": 1, "owner": 7, "owner_name": "玩家".encode(), "price": -1, "instant_price": 24000,
                         "unit_price": 24000, "item_id": 10478, "count": 7, "expire": 0, "raw": info}
        ro = bytearray(P.equipment_item_info(20203, 1, 0, 9, b""))
        ro[29:32] = b"\x1d\x09\x14"
        m.listings[2] = {"auction_id": 2, "owner": 7, "owner_name": "玩家".encode(), "price": -1, "instant_price": 1000,
                         "unit_price": 1000, "item_id": 20203, "count": 9, "expire": 0, "raw": bytes(ro)}
        bot.watch_ids |= {10478, 20203}
        for _ in range(4):
            await bot.runner.step()
        self.assertIn(1, m.listings)                                # 收购关闭：玩家装备没被买
        self.assertIn(2, m.listings)                                # 假紫从不买
        mine = [l for l in list(m.listings.values()) + list(m.sold.values()) if l["owner"] in (-1, -114514)]
        self.assertGreater(len(mine), 0)
        for l in mine:
            u = P.unpack_item_info(l["raw"])
            self.assertIn(l["item_id"], eq.catalog)
            self.assertEqual(u["upgrade"], 0)
            self.assertTrue(u["random_empty"])
            self.assertEqual(u["endurance"], eq.catalog[l["item_id"]]["endurance"])
            self.assertEqual(u["seal"], eq.catalog[l["item_id"]]["seal"])
            self.assertEqual(l["price"], -1)
            self.assertEqual(l["instant_price"], l["unit_price"])
            lo, hi = eq.band(l["item_id"])
            self.assertTrue(lo <= l["instant_price"] <= hi)
        guids = [P.unpack_item_info(l["raw"])["guid"] for l in mine]
        self.assertEqual(len(set(guids)), len(guids))
        # 每层级挂单不超过放量上限
        for t in eq.tiers:
            n = sum(1 for l in m.listings.values() if l["owner"] in (-1, -114514) and eq.tier_of(l["item_id"]) == t)
            self.assertLessEqual(n, 2)
        # 虚拟成交只买机器人自己的装备，买家 = 机器人 -114514
        self.assertTrue(all(h[2] == -114514 for h in m.history))
        self.assertTrue(all(h[1] not in (1, 2) for h in m.history))
        # 开启收购后可以买 +0 玩家装备（受独立日上限），假紫依旧不买；材料 2M 上限不受影响
        bot.cfg["engine"]["equipment"]["user_fill"] = {"enabled": True, "max_gold_per_day": 30000}
        for _ in range(6):
            await bot.runner.step()
        self.assertIn(2, m.listings)
        spent = bot.store.query("SELECT SUM(total) g FROM deals WHERE side='bot_buy_human'")[0]["g"] or 0
        self.assertLessEqual(spent, 30000)
        # 扫描：装备数量按 1 计，不会把 add_info 当数量
        await bot.scan_market()
        obs = bot.store.query("SELECT MAX(count) c FROM listing_obs WHERE item_id IN (10478, 20203)")[0]["c"]
        self.assertEqual(obs, 1)
        # 手动上架：未开 manual_enabled → 拒绝；开启后只允许清单内装备
        self.assertTrue(manual_preview(bot, 10478)["blocked"])
        bot.cfg["engine"]["equipment"]["manual_enabled"] = True
        self.assertIsNone(manual_preview(bot, 10478)["blocked"])
        self.assertTrue(manual_preview(bot, 20203)["blocked"])
        r = await manual_list(bot, 20203, 1, 30000)
        self.assertFalse(r["ok"])
        r = await manual_list(bot, 10478, 2, 30000)
        self.assertTrue(r["ok"], r)
        self.assertEqual([c["count"] for c in r["created"]], [1, 1])
        for c in r["created"]:
            l = m.listings[c["auction_id"]]
            self.assertEqual(P.unpack_item_info(l["raw"])["seal"], 1)
        r = await manual_list(bot, 10478, 1, 10 ** 9)
        self.assertFalse(r["ok"])                                   # 偏离价格带太多
        st = bot.runner.status()["equipment"]
        self.assertEqual(st["items"], 3)
        await bot.client.close()
        srv.close()
        self.assertEqual(m.violations, [])


class TestEquipRandomOption(unittest.TestCase):
    """假紫：random_option 段开启+verified 才进清单；挂单带 give_option 生成的属性 + ROI 30000；sealed 模式属性全 0。"""

    def _bot(self, port, ro):
        b = TestEquipRunner._bot(self, port, {"enabled": True, "format_verified": True, "max_listings_per_tier": 3,
                                              "volume_scale": 400.0, "max_new_per_step": 10, "max_new_per_hour": 100,
                                              "virtual_fill": {"max_per_step": 10, "max_per_day": 100},
                                              "user_fill": {"enabled": False}, "manual_enabled": True, "random_option": ro})
        from tests.test_randopt import fixture
        b.runner.equip.ro_tables = fixture()
        b.runner.reload()
        return b

    def test_gate(self):
        self.assertNotIn(20203, self._bot(1, {"enabled": True, "verified": False}).runner.equip.catalog)
        self.assertNotIn(20203, self._bot(1, {"enabled": False, "verified": True}).runner.equip.catalog)
        bot = self._bot(1, {"enabled": True, "verified": True})
        eq = bot.runner.equip
        self.assertIn(20203, eq.catalog)
        self.assertNotIn(2200110, eq.catalog)                          # csv 与 PVF 不一致 → 不挂
        # v4.2 的假紫 listing.random_option 是说明文字，不能因此被跳过；普通装备带预设属性仍跳过
        it = dict(ITEMS["20203"], listing=dict(ITEMS["20203"]["listing"], random_option="机器人按服务器规则生成"))
        self.assertIsNone(eq._check(20203, it, {"EQ_A": TIERS["EQ_A"]}))
        it = dict(ITEMS["10478"], listing=dict(ITEMS["10478"]["listing"], random_option="机器人按服务器规则生成"))
        self.assertIn("预设随机属性", eq._check(10478, it, {"EQ_A": TIERS["EQ_A"]}))
        self.assertEqual(eq.catalog[20203]["ro_args"], (2, 50, 51, 1))
        info, f = eq.item_info(20203)
        u = P.unpack_item_info(info)
        self.assertFalse(u["random_empty"])
        self.assertEqual(u["random_option"], f["random_option"])
        self.assertEqual(eq.ro_tables.explain(bytes.fromhex(u["random_option"]), 2, 50, 51, 1), [])
        self.assertEqual((u["seal"], u["endurance"], u["upgrade"]), (1, 0, 0))
        self.assertEqual(f["roi"], struct.pack("<hhhH", 30000, 30000, 30000, 0) + b"\0" * 4)
        info, f = eq.item_info(10478)
        self.assertTrue(P.unpack_item_info(info)["random_empty"])
        self.assertEqual(f["roi"], b"\0" * 12)
        bot.cfg["engine"]["equipment"]["random_option"]["mode"] = "sealed"
        info, f = eq.item_info(20203)
        self.assertTrue(P.unpack_item_info(info)["random_empty"])
        self.assertEqual(f["roi"][:2], struct.pack("<h", 30000))

    def test_mock_e2e(self):
        asyncio.run(self._run())

    async def _run(self):
        from auction_bot.client import AuctionClient
        from auction_bot.runner import manual_list
        for k in (10478, 20066, 31119, 20203, 2200110, 3274):
            mock_server.KNOWN_ITEMS[k] = k == 3274
        m, srv, port = await mock_server.start()
        bot = self._bot(port, {"enabled": True, "verified": True})
        bot.loop = asyncio.get_running_loop()
        bot.client = AuctionClient("127.0.0.1", port, lambda *a, **k: None, 3, 0)
        await bot.client.connect()
        eq = bot.runner.equip
        r = await manual_list(bot, 20203, 2, 30000)
        self.assertTrue(r["ok"], r)
        for _ in range(4):
            await bot.runner.step()
        ro = [l for l in list(m.listings.values()) + list(m.sold.values()) if l["owner"] in (-1, -114514) and l["item_id"] == 20203]
        self.assertGreaterEqual(len(ro), 2)
        for l in ro:
            u = P.unpack_item_info(l["raw"])
            self.assertFalse(u["random_empty"])
            self.assertEqual(eq.ro_tables.explain(bytes.fromhex(u["random_option"]), 2, 50, 51, 1), [])
            self.assertEqual(l["roi"][:6], struct.pack("<hhh", 30000, 30000, 30000))
        for l in m.listings.values():
            if l["owner"] in (-1, -114514) and l["item_id"] != 20203:
                self.assertEqual(l["roi"], b"\0" * 12)
        await bot.client.close()
        srv.close()
        self.assertEqual(m.violations, [])


class TestEquipShelf(unittest.TestCase):
    """v4.2 上架时长模型：补挂到 target_visible_listings；挂出不满 min_shelf_minutes 绝不会被虚拟买家买；之后按风险率买。"""

    SHELF = {"min_shelf_minutes": 30, "buy_hazard_per_h": 0.33, "price_sensitivity": 7.5, "target_sell_through": 0.35,
             "target_visible_listings": 3}

    def _bot(self, port, shelf=None, extra=None):
        import copy
        global TIERS
        saved = TIERS
        TIERS = copy.deepcopy(saved)
        TIERS["EQ_A"]["shelf"] = dict(self.SHELF, **(shelf or {}))
        try:
            cfg = {"enabled": True, "format_verified": True, "max_listings_per_tier": None, "volume_scale": 1.0,
                   "max_new_per_step": 10, "max_new_per_hour": 100, "virtual_fill": {"max_per_step": 10, "max_per_day": 100},
                   "user_fill": {"enabled": False}}
            cfg.update(extra or {})
            return TestEquipRunner._bot(self, port, cfg)
        finally:
            TIERS = saved

    def test_formula_and_target(self):
        from auction_bot.engine import online_mean
        bot = self._bot(1)
        eq = bot.runner.equip
        sh = eq.shelf("EQ_A")
        self.assertEqual(sh["target_visible_listings"], 3)
        self.assertEqual(sh["price_sensitivity"], 7.5)
        self.assertIsNone(eq.shelf("EQ_HI"))                       # 没有 shelf 参数的层级走旧模型
        self.assertGreater(eq.online_mean, 0)
        self.assertAlmostEqual(eq.online_mean, online_mean(bot.runner.model["global"]))
        bot.cfg["engine"]["equipment"]["max_listings_per_tier"] = 1
        self.assertEqual(eq.shelf("EQ_A")["target_visible_listings"], 1)
        bot.cfg["engine"]["equipment"]["shelf"] = {"enabled": False}
        self.assertIsNone(eq.shelf("EQ_A"))
        self.assertEqual(self._bot(1, extra={"shelf": {"override": {"min_shelf_minutes": 45}}}).runner.equip.shelf("EQ_A")["min_shelf_minutes"], 45)

    def test_shelf_against_mock(self):
        asyncio.run(self._run())

    async def _run(self):
        from auction_bot.client import AuctionClient
        for k in (10478, 20066, 31119, 20203, 2200110, 3274):
            mock_server.KNOWN_ITEMS[k] = k == 3274
        m, srv, port = await mock_server.start()
        bot = self._bot(port, {"buy_hazard_per_h": 1e6})          # 风险率极大：只要过了最短上架时间就必买
        bot.loop = asyncio.get_running_loop()
        bot.client = AuctionClient("127.0.0.1", port, lambda *a, **k: None, 3, 0)
        await bot.client.connect()
        eq = bot.runner.equip

        def sys_a():
            return [l for l in m.listings.values() if l["owner"] in (-1, -114514) and eq.tier_of(l["item_id"]) == "EQ_A"]
        await bot.runner.step()
        self.assertEqual(len(sys_a()), 3)                          # 补到目标
        self.assertEqual(m.history, [])                            # 同一步不成交
        for _ in range(3):
            await bot.runner.step()
        self.assertEqual(len(sys_a()), 3)                          # 不会超挂
        self.assertEqual(m.history, [])                            # 不满 30 分钟，风险率再大也不买
        st = eq.status()
        self.assertEqual([x for x in st["tiers"] if x["tier"] == "EQ_A"][0]["visible"], 3)
        self.assertEqual(st["visible_total"], 3 + [x for x in st["tiers"] if x["tier"] == "EQ_HI"][0]["visible"])
        for l in sys_a():
            lo, hi = eq.band(l["item_id"])
            self.assertTrue(lo <= l["instant_price"] <= hi)
            self.assertEqual(l["price"], -1)
        # 把其中两张的上架时间往前拨 31 分钟 → 下一步被买走，再补挂两张新的
        old = sorted(l["auction_id"] for l in sys_a())[:2]
        bot.store.execute("UPDATE bot_listings SET listed_at=listed_at-1860 WHERE auction_id IN (%d,%d)" % tuple(old))
        await bot.runner.step()
        self.assertEqual(sorted(h[1] for h in m.history), old)
        self.assertTrue(all(h[2] == -114514 for h in m.history))
        self.assertEqual(len(sys_a()), 1)
        self.assertEqual(eq.last_shelf["EQ_A"]["sold"], 2)
        self.assertEqual(eq.last_shelf["EQ_A"]["listed"], 0)    # 卖出的下一步才补
        await bot.runner.step()
        self.assertEqual(eq.last_shelf["EQ_A"]["listed"], 2)
        self.assertEqual(len(sys_a()), 3)
        # 价格过高 → attr 极小 → 不买（价格敏感度）
        for l in sys_a():
            bot.store.execute("UPDATE bot_listings SET listed_at=listed_at-1860, instant_price=instant_price*100 WHERE auction_id=?",
                              (l["auction_id"],))
        bot.runner.equip.rng.seed(1)
        n = len(m.history)
        eq.cfg["shelf"] = {"override": {"buy_hazard_per_h": 0.33}}
        await bot.runner.step()
        self.assertEqual(len(m.history), n)
        # 虚拟成交只买系统挂单，从不碰玩家的
        self.assertTrue(all(m.sold[h[1]]["owner"] in (-1, -114514) for h in m.history))
        await bot.client.close()
        srv.close()
        self.assertEqual(m.violations, [])


if __name__ == "__main__":
    unittest.main(verbosity=2)
