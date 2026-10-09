# -*- coding: utf-8 -*-
"""随机属性（假紫）生成：复刻 give_option 的查表/抽取规则；真实 PVF 存在时（WSL）再用库里真实解封装备做 ground truth。"""
import os
import random
import sys
import unittest

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, BASE)
from auction_bot import randopt as R           # noqa: E402

PVF_PATH = "/opt/dnf1031/data/Script.pvf"


def tok(*xs):
    out = []
    for x in xs:
        if isinstance(x, str) and x.startswith("["):
            out.append((5, x))
        elif isinstance(x, str):
            out.append((7, x))
        elif isinstance(x, float):
            out.append((4, x))
        else:
            out.append((2, x))
    return out


def fixture():
    o1 = ["[option type]"]
    q = ["[quantity ratio]", 1, 10000, 2, 5000, 3, 900, "[/quantity ratio]", "[option quantity]"]
    sel = ["[choose option group]"]
    for g in (1, 2, 3):
        q += [2, -1, g, 50, 1, 3]
        sel += [2, -1, "amulet", g, 50, 2, 301, 302, 100, 200]
        sel += [2, -1, "cl coat", g, 50, 3, 303, 303, 304, 0, 0, 300]
    o1 += [2, -1, 50, 7000, 7000, 2500, "[/option type]"]
    q += ["[/option quantity]"]
    sel += ["[/choose option group]"]
    files = {
        "overall1": tok(*o1), "quantity": tok(*q), "selection": tok(*sel),
        "numbering": tok("[option value ratio]", 0.1, 0.3, 25, 0.4, 0.5, 140, 0.6, 0.7, 25000, 0.8, 1.0, 50000, "[/option value ratio]"),
        "group": tok("[option group]", 301, 15, 10000, 16, 10000, 19, 5000, "[/option group]",
                     "[option group]", 302, 17, 10000, 18, 10000, 20, 5000, 21, 100, "[/option group]",
                     "[option group]", 304, 29, 10000, 71, 10000, 74, 10000, "[/option group]"),
        "overall2": tok("[break seal cost]", 2, 50, 1, 520, "[different weight]", 2, 1, 0, 25, 2, 2, 10, 30, 2, 3, 30, 55,
                        "[/different weight]"),
    }
    return R.RandomOptionTables.from_tokens(files, tok(26, "cl coat", 51, "amulet"))


class TestRandomList(unittest.TestCase):
    def test_crandom_list_quirks(self):
        self.assertEqual(R.crandom_list([10, 20, 30], 0), 0)
        self.assertEqual(R.crandom_list([10, 20, 30], 9), 0)
        self.assertEqual(R.crandom_list([10, 20, 30], 10), 1)
        self.assertEqual(R.crandom_list([10, 20, 30], 59), 2)
        self.assertEqual(R.crandom_list([10, 20, 30], 60), 0)     # randInt 闭区间：抽到总权重 → 不命中 → 下标 0
        self.assertEqual(R.crandom_list([0, 0, 300], 0), 2)
        self.assertEqual(R.crandom_list([0, 0, 300], 299), 2)


class TestGiveOption(unittest.TestCase):
    def setUp(self):
        self.t = fixture()
        self.amulet = self.t.group_names["amulet"]

    def test_tables(self):
        self.assertEqual(self.t.different_weight[(2, 2)], (10, 30))
        self.assertEqual(self.t.break_seal_cost[(2, 50, 1)], 520)
        self.assertIsNone(self.t.supports(2, 50, self.amulet, 1))
        self.assertIn("option_type", self.t.supports(2, 45, self.amulet, 1))
        self.assertIn("choose_option_group", self.t.supports(2, 50, 99, 1))

    def test_generated_bytes(self):
        rng = random.Random(7)
        seen_q, seen_g = set(), set()
        for _ in range(3000):
            b, info = self.t.give_option(2, 50, self.amulet, 1, rng)
            self.assertEqual(len(b), 14)
            self.assertEqual(b[10:], b"\0\0\0\0")                       # modify 区解封时不写
            self.assertEqual(b[9] & 3, info["grade"])
            self.assertTrue(1 <= info["grade"] <= 3)
            types = [b[i * 3] for i in range(3) if b[i * 3]]
            self.assertEqual(len(types), info["quantity"])
            grp = {301: {15, 16, 19}, 302: {17, 18, 20, 21}}[info["group"]]
            self.assertTrue(set(types) <= grp)
            self.assertEqual(len(set(types)), len(types))                # 组内不放回
            mn, mx = self.t.different_weight[(2, info["grade"])]
            for i in range(len(types)):
                for v in b[i * 3 + 1:i * 3 + 3]:
                    self.assertTrue(int(mn * 0.1) <= v <= int(mn * 0.8) + (mx - mn), (v, mn, mx))
            self.assertEqual(self.t.explain(b, 2, 50, self.amulet, 1), [])
            seen_q.add(info["quantity"])
            seen_g.add(info["grade"])
        self.assertEqual(seen_q, {1, 2, 3})
        self.assertEqual(seen_g, {1, 2, 3})
        # 权重 0 的组永远不会被选
        for _ in range(300):
            b, info = self.t.give_option(2, 50, self.t.group_names["cl coat"], 1, rng)
            self.assertEqual(info["group"], 304)

    def test_explain_rejects(self):
        self.assertTrue(self.t.explain(bytes.fromhex("63010100000000000001" + "00" * 4), 2, 50, self.amulet, 1))   # 类型不在组
        self.assertTrue(self.t.explain(bytes.fromhex("0f010100000000000004" + "00" * 4), 2, 50, self.amulet, 1))   # 档次 0
        self.assertTrue(self.t.explain(b"\0" * 14, 2, 50, self.amulet, 1))

    def test_roi(self):
        self.assertEqual(R.roi_bytes_for(False), b"\0" * 12)
        r = R.roi_bytes_for(True)
        self.assertEqual(len(r), 12)
        self.assertEqual(int.from_bytes(r[:8], "little"), 128850984990000)   # = 模板库 auction_roi_average_price.roi_high_key
        self.assertEqual(r[8:], b"\0" * 4)


@unittest.skipUnless(os.path.exists(PVF_PATH), "需要真实 PVF（WSL）")
class TestRealPVF(unittest.TestCase):
    """测试角色身上 6 件已解封的随机属性装备（2026-10-10 从 inventory.equipslot 读出）必须是 give_option 可能生成的结果。"""
    GT = {2221103: "1d0914000000000000a600000000", 2290103: "4710114a0d170000008a00000000",
          2292103: "4c18180000000000002d00000000", 2291103: "480a130000000000002200000000",
          2294103: "581211000000000000f200000000", 2293103: "54301a000000000000b300000000"}

    def test_ground_truth(self):
        from auction_bot.pvf import PVF, load_item_index
        import tempfile
        t = R.RandomOptionTables.from_pvf(PVF(PVF_PATH))
        idx = load_item_index(PVF_PATH, os.path.join(tempfile.mkdtemp(), "i.json"))
        for iid, h in self.GT.items():
            m = idx[iid]
            g = t.group_names[m["item_group"].strip()]
            self.assertEqual(t.explain(bytes.fromhex(h), m["rarity"], m["level"], g, m["ro_grade"] or 1), [], iid)
        rng = random.Random(3)
        n = 0
        for iid, m in idx.items():
            if not m.get("random_option") or m["level"] not in (30, 40, 50, 60, 70):
                continue
            g = t.group_names[(m["item_group"] or "").strip()]
            if t.supports(m["rarity"], m["level"], g, m["ro_grade"] or 1):
                continue
            for _ in range(20):
                b, _i = t.give_option(m["rarity"], m["level"], g, m["ro_grade"] or 1, rng)
                self.assertEqual(t.explain(b, m["rarity"], m["level"], g, m["ro_grade"] or 1), [])
            n += 1
        self.assertGreater(n, 100)


if __name__ == "__main__":
    unittest.main(verbosity=2)
