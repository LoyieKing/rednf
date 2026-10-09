# -*- coding: utf-8 -*-
"""
随机属性（假紫装备）生成 —— 逐行复刻 ORIG df_game_r 的 random_option::CRandomOptionItemHandle::give_option。

ORIG 事实（remake 源码 RandomRentalA2/*.inc、Dispatchers/dispatch_sig_116.inc、CDataManager/init.inc）：
  * 怪物掉落 / 拾取（CParty::get_item → CEquipItem::make_item）**不生成**随机属性：刚掉落的假紫 RandomOption 14 字节全 0
    （= 未鉴定/封印状态，move_item 拒绝穿戴 options 为空的随机属性装备，错误 0x11）。
  * 玩家在「随机属性解封」（Dispatcher_UnsealRandomOption，花 [break seal cost] 金币或解封券）时，服务器调用
        give_option(itemIdx, rarity=[rarity], level=[minimum level], group=[item group name] 序号, grade=[random option] 值, opts)
    其它调用者：CCeraShop_Special::autoUnsealItem、eventReward::unsealRandomOption、W8_CU_A::lottery_item —— 同一函数。
  * 读取的 PVF 脚本（CDataManager::init 七文件循环 importRandomOptionScript）：
        etc/randomoption/optiongrouping.etc            [option group]        组号 → (属性类型, 权重)…
        etc/randomoption/optiongroupselection.etc      [choose option group] (稀有度, 等级组, 部位组, 档次, 等级) → 组号/权重
        etc/randomoption/optionnumbering.etc           [option value ratio]  (base 系数, range 系数, 权重)
        etc/randomoption/optionquantity.etc            [quantity ratio] / [option quantity]
        etc/randomoption/partselection.etc             （give_option 不用）
        etc/randomoption/randomizedoptionoverall1.etc  [option type]         (稀有度, 等级组, 等级) → 档次 1..3 权重
        etc/randomoption/randomizedoptionoverall2.etc  [different weight]    (稀有度, 档次) → (min, max)；[break seal cost]
    部位组名 → 序号：etc/itemgroupname.tbl。
  * give_option 步骤（m_rand[k] 是独立的 CMTRand，randInt(n) 返回 [0, n] 闭区间）：
      1 档次 outGrade：[option type] 键 (rarity, level)，内层先找 -1 再找 grade；按 3 个权重 CRandomList 抽，+1。
      2 条数：[option quantity] 键 {rarity, 0xff→grade, outGrade, level} → (min,max)；权重取 [quantity ratio] 第 min..max 项。
      3 组号：[choose option group] 键 {rarity, 0xff→grade, 部位组, level} + outGrade；r = randInt(总权重-1)。
      4 (min,max) = [different weight][(rarity, outGrade)]。
      5 每条（最多 3 条）：从组里按权重不放回抽属性类型；两个数值各 _get_option_value(min,max)：
           按 [option value ratio] 抽 (base, range)，值 = int(min*base) + randInt(int((max-min)*range))。
      6 seed 字节 = (outGrade & 3) | ((randInt(64) & 0x3f) << 2)。
  * RandomOption 14 字节：3 × {类型 u8, 数值1 u8, 数值2 u8} @0..8，seed @9（低 2 位 = 档次，高 6 位 = 随机种子），
    modify_option[3] @10..12 与 modify_seed @13 只有「属性变更/再生成」才写，解封后为 0。
  * CRandomList 的原版怪癖也照抄：抽中值 == 总权重时（randInt 闭区间）不命中任何项，返回下标 0。
"""
import random
import struct

RANDOM_OPTION_FILES = {
    "group": "etc/randomoption/optiongrouping.etc",
    "selection": "etc/randomoption/optiongroupselection.etc",
    "numbering": "etc/randomoption/optionnumbering.etc",
    "quantity": "etc/randomoption/optionquantity.etc",
    "overall1": "etc/randomoption/randomizedoptionoverall1.etc",
    "overall2": "etc/randomoption/randomizedoptionoverall2.etc",
}
ITEM_GROUP_TBL = "etc/itemgroupname.tbl"
MAX_RANDOM_SEED = 64
ROI_UNSEALED_KEY = 30000      # 游戏端 dispatch_sig（登记拍卖）：随机属性装备的 ROI 空槽填 0x7530；拍卖服 _static_UnsealRandomOptionItem 同值


class RandomOptionError(Exception):
    pass


def _f32(x):
    return struct.unpack("<f", struct.pack("<f", float(x)))[0]


def _sections(tokens):
    """PVF 词元 → [(标签, [值...])]，按出现顺序（同名标签可出现多次，如 [option group]）。"""
    out = []
    cur = None
    for t, v in tokens:
        if t == 5:
            tag = str(v).lower()
            if tag.startswith("[/"):
                cur = None
                continue
            cur = (tag, [])
            out.append(cur)
            continue
        if cur is not None:
            cur[1].append((t, v))
    return out


def _ints(vals):
    return [int(v) for t, v in vals]


def crandom_list(weights, n):
    """ORIG CRandomList<unsigned>：for_each 累加；m_sum>n 后不再处理；n-m_sum < w 时记下标（无命中 → 0）。"""
    value, s = 0, 0
    for i, w in enumerate(weights):
        w = int(w) & 0xffffffff
        if s > n:
            break
        if (n - s) & 0xffffffff < w:
            value = i
        s = (s + w) & 0xffffffff
    return value


def crandom_list_ex(items, n):
    """ORIG CRandomListEx<OptionValueRatio>：同上，但返回元素本身；无命中 → 默认构造（全 0）。"""
    sel, s = (0.0, 0.0, 0), 0
    for it in items:
        w = it[2]
        if s > n:
            break
        if (n - s) & 0xffffffff < w:
            sel = it
        s = (s + w) & 0xffffffff
    return sel


class RandomOptionTables:
    def __init__(self):
        self.option_type = {}      # (rarity, level) -> {optType: [w1,w2,w3]}
        self.quantity_ratio = []   # [(n, weight)]
        self.option_quantity = {}  # (rarity, b1, grade, level) -> (min, max)
        self.choose_group = {}     # (rarity, b1, group, level, outGrade) -> (ids, weights, total)
        self.value_ratio = []      # [(range, base, weight)]
        self.value_ratio_total = 0
        self.option_group = {}     # groupId -> [(type, weight)]
        self.different_weight = {} # (rarity, outGrade) -> (min, max)
        self.break_seal_cost = {}  # (rarity, level, grade) -> gold
        self.group_names = {}      # name -> idx

    # ---------- 载入 ----------
    @classmethod
    def from_tokens(cls, files, group_tbl_tokens):
        """files: {key: [(type, value)...]}（PVF.units 的输出）；group_tbl_tokens: itemgroupname.tbl 的词元。"""
        self = cls()
        vals = [(t, v) for t, v in group_tbl_tokens if t != 5]
        for i in range(0, len(vals) - 1, 2):
            self.group_names[str(vals[i + 1][1])] = int(vals[i][1])
        for key in ("group", "selection", "numbering", "quantity", "overall1", "overall2"):
            for tag, v in _sections(files[key]):
                getattr(self, "_sec_" + tag.strip("[]").replace(" ", "_"), lambda v: None)(v)
        self.check()
        return self

    @classmethod
    def from_pvf(cls, pvf):
        files = {k: pvf.units(pvf.read(p)) for k, p in RANDOM_OPTION_FILES.items()}
        return cls.from_tokens(files, pvf.units(pvf.read(ITEM_GROUP_TBL)))

    def _sec_option_type(self, v):
        x = _ints(v)
        for i in range(0, len(x) - 5, 6):
            rar, ot, lv, w1, w2, w3 = x[i:i + 6]
            d = self.option_type.setdefault((rar & 0xff, lv & 0xff), {})
            if ot in d:
                raise RandomOptionError("option_type duplicate")
            d[ot] = [w1, w2, w3]

    def _sec_quantity_ratio(self, v):
        x = _ints(v)
        self.quantity_ratio = [(x[i], x[i + 1]) for i in range(0, len(x) - 1, 2)]

    def _sec_option_quantity(self, v):
        x = _ints(v)
        for i in range(0, len(x) - 5, 6):
            k = tuple(b & 0xff for b in x[i:i + 4])
            if k in self.option_quantity:
                raise RandomOptionError("option_quantity duplicate")
            self.option_quantity[k] = (x[i + 4], x[i + 5])

    def _sec_choose_option_group(self, v):
        i = 0
        while i < len(v):
            rar, b1 = int(v[i][1]), int(v[i + 1][1])
            name = str(v[i + 2][1])
            if name not in self.group_names:
                raise RandomOptionError("item group name %r 不在 itemgroupname.tbl" % name)
            hi, lv, n = int(v[i + 3][1]), int(v[i + 4][1]), int(v[i + 5][1])
            ids = [int(v[i + 6 + j][1]) for j in range(n)]
            ws = [int(v[i + 6 + n + j][1]) for j in range(n)]
            k = (rar & 0xff, b1 & 0xff, self.group_names[name] & 0xff, lv & 0xff, hi)
            if k in self.choose_group:
                raise RandomOptionError("option_group_choice duplicate")
            self.choose_group[k] = (ids, ws, sum(ws))
            i += 6 + 2 * n

    def _sec_option_value_ratio(self, v):
        for i in range(0, len(v) - 2, 3):
            base, rng, w = _f32(v[i][1]), _f32(v[i + 1][1]), int(v[i + 2][1])
            self.value_ratio.append((rng, base, w))
            self.value_ratio_total += w

    def _sec_option_group(self, v):
        x = _ints(v)
        gid, rest = x[0], x[1:]
        g = []
        for i in range(0, len(rest) - 1, 2):
            if not (0 < rest[i] <= 0xff):
                raise RandomOptionError("option group type out of range")
            g.append((rest[i], rest[i + 1]))
        if gid in self.option_group:
            raise RandomOptionError("option_group duplicate")
        self.option_group[gid] = g

    def _sec_different_weight(self, v):
        x = _ints(v)
        for i in range(0, len(x) - 3, 4):
            mn, mx = x[i + 2], x[i + 3]
            if mx <= mn or not (0 <= mn <= 100 and 0 <= mx <= 100):
                raise RandomOptionError("different_weight 范围非法")
            self.different_weight[(x[i] & 0xff, x[i + 1] & 0xff)] = (mn, mx)

    def _sec_break_seal_cost(self, v):
        x = _ints(v)
        for i in range(0, len(x) - 3, 4):
            self.break_seal_cost[(x[i], x[i + 1], x[i + 2])] = x[i + 3]

    def check(self):
        for name in ("option_type", "quantity_ratio", "option_quantity", "choose_group", "value_ratio",
                     "option_group", "different_weight", "group_names"):
            if not getattr(self, name):
                raise RandomOptionError("随机属性脚本缺少 %s" % name)

    def summary(self):
        return {"option_type": len(self.option_type), "option_quantity": len(self.option_quantity),
                "choose_group": len(self.choose_group), "option_group": len(self.option_group),
                "value_ratio": [(round(r, 3), round(b, 3), w) for r, b, w in self.value_ratio],
                "different_weight": {"%d/%d" % k: v for k, v in sorted(self.different_weight.items())},
                "quantity_ratio": self.quantity_ratio}

    # ---------- give_option ----------
    def _give(self, rarity, level):
        return self.option_type.get((rarity & 0xff, level & 0xff))

    def _quantity_key(self, rarity, grade, out_grade, level):
        for b1 in (0xff, grade & 0xff):
            k = (rarity & 0xff, b1, out_grade & 0xff, level & 0xff)
            if k in self.option_quantity:
                return self.option_quantity[k]
        return None

    def _choice(self, rarity, grade, group, level, out_grade):
        for b1 in (0xff, grade & 0xff):
            k = (rarity & 0xff, b1, group & 0xff, level & 0xff, out_grade)
            if k in self.choose_group:
                return self.choose_group[k]
        return None

    def supports(self, rarity, level, group, grade):
        """这件装备能不能被服务器解封（give_option 不会因为查表失败而返回 false）。"""
        give = self._give(rarity, level)
        if not give:
            return "option_type 无 (%d,%d)" % (rarity, level)
        g = give.get(-1) or give.get(grade)
        if g is None:
            return "option_type 无档次表"
        for og in (1, 2, 3):
            if g[og - 1] <= 0:
                continue
            q = self._quantity_key(rarity, grade, og, level)
            if not q or q[1] == 0:
                return "option_quantity 无 (%d,%d,%d)" % (rarity, og, level)
            ch = self._choice(rarity, grade, group, level, og)
            if not ch:
                return "choose_option_group 无 (%d,%d,%d,%d)" % (rarity, group, level, og)
            if (rarity & 0xff, og) not in self.different_weight:
                return "different_weight 无 (%d,%d)" % (rarity, og)
            for gid, w in zip(ch[0], ch[1]):
                if w > 0 and gid not in self.option_group:
                    return "option_group %d 不存在" % gid
        return None

    def _value(self, rng, mn, mx):
        r1 = rng.randint(0, self.value_ratio_total)
        rr, bb, _ = crandom_list_ex(self.value_ratio, r1)
        span = int(float(mx - mn) * rr)          # 精确乘积（double 能精确表示 f32×小整数）后向零截断
        base = int(float(mn) * bb)
        return (base + rng.randint(0, max(span, 0))) & 0xff

    def give_option(self, rarity, level, group, grade, rng=None):
        """返回 (14 字节, 说明)。查表失败抛 RandomOptionError（服务器同样返回 false，不会给属性）。"""
        rng = rng or random.SystemRandom()
        give = self._give(rarity, level)
        if give is None:
            raise RandomOptionError("_get_option_type 0x23f")
        g = give.get(-1)
        if g is None:
            g = give.get(grade)
        if g is None:
            raise RandomOptionError("_get_option_type 0x255")
        out_grade = (crandom_list(g, rng.randint(0, sum(g))) + 1) & 0xff
        q = self._quantity_key(rarity, grade, out_grade, level)
        if q is None:
            raise RandomOptionError("_get_option_quantity 0x26c")
        mn_q, mx_q = q
        if mx_q == 0:
            raise RandomOptionError("_get_option_quantity 0x270")
        ws = [w for _, w in self.quantity_ratio[mn_q - 1:mx_q]]
        if not ws:
            raise RandomOptionError("_get_option_quantity 0x27d")
        if len(ws) == 1:
            quantity = mn_q
        else:
            quantity = crandom_list(ws, rng.randint(0, sum(ws))) + mn_q
        ch = self._choice(rarity, grade, group, level, out_grade)
        if ch is None:
            raise RandomOptionError("_choose_option_group 0x29e")
        ids, cws, total = ch
        sel = crandom_list(cws, rng.randint(0, max(total - 1, 0)))
        if sel >= len(ids):
            raise RandomOptionError("_choose_option_group 0x2be")
        gid = ids[sel]
        dw = self.different_weight.get((rarity & 0xff, out_grade))
        if dw is None:
            raise RandomOptionError("different_weight 缺失")
        og = self.option_group.get(gid)
        if og is None:
            raise RandomOptionError("option_group 缺失 %d" % gid)
        remaining = sum(w for _, w in og)
        vec = list(og)
        b = bytearray(14)
        opts = []
        slot = 0
        while slot < quantity and slot <= 2:
            pick = rng.randint(0, max(remaining, 0))
            s = crandom_list([w for _, w in vec], pick)
            if s < len(vec):
                typ, w = vec.pop(s)
                remaining -= w
                v1 = self._value(rng, dw[0], dw[1])
                v2 = self._value(rng, dw[0], dw[1])
                b[slot * 3:slot * 3 + 3] = bytes((typ & 0xff, v1, v2))
                opts.append((typ, v1, v2))
            slot += 1
        seed = (out_grade & 3) | ((rng.randint(0, MAX_RANDOM_SEED) & 0x3f) << 2)
        b[9] = seed
        return bytes(b), {"grade": out_grade, "quantity": quantity, "group": gid, "options": opts, "seed": seed >> 2,
                          "range": dw}

    # ---------- 校验（用真实物品做 ground truth） ----------
    def explain(self, ro, rarity, level, group, grade):
        """判断一段 14 字节随机属性是否可能由 give_option(rarity, level, group, grade) 生成。返回问题列表（空 = 可能）。"""
        ro = bytes(ro)
        probs = []
        if len(ro) != 14:
            return ["长度不是 14"]
        if any(ro[10:14]):
            probs.append("modify_option/modify_seed 非 0（属性变更过）")
        og = ro[9] & 3
        give = self._give(rarity, level) or {}
        g = give.get(-1) or give.get(grade)
        if not g or og < 1 or og > 3 or g[og - 1] <= 0:
            return probs + ["档次 %d 不可能" % og]
        q = self._quantity_key(rarity, grade, og, level)
        opts = [tuple(ro[i * 3:i * 3 + 3]) for i in range(3)]
        n = sum(1 for o in opts if o[0])
        if any(o[0] == 0 and any(o) for o in opts):
            probs.append("空槽有数值")
        if opts[0][0] == 0 or (opts[1][0] == 0 and opts[2][0]):
            probs.append("槽位不连续")
        if q is None or not (q[0] <= n <= q[1]):
            probs.append("条数 %d 不在 %s" % (n, q))
        ch = self._choice(rarity, grade, group, level, og)
        if ch is None:
            return probs + ["无 choose_option_group"]
        types = [o[0] for o in opts if o[0]]
        need = {}
        for t in types:
            need[t] = need.get(t, 0) + 1

        def fits(gid):   # 组里可以重复列同一类型（不放回抽取按条目算），按多重集合比较
            have = {}
            for t, w in self.option_group.get(gid, []):
                if w > 0:
                    have[t] = have.get(t, 0) + 1
            return all(have.get(t, 0) >= n for t, n in need.items())
        ok_group = [gid for gid, w in zip(ch[0], ch[1]) if w > 0 and fits(gid)]
        if not ok_group:
            probs.append("属性类型 %s 不在任何候选组 %s" % (types, sorted(set(ch[0]))))
        mn, mx = self.different_weight[(rarity & 0xff, og)]
        vmax = max(int(float(mn) * b) + int(float(mx - mn) * r) for r, b, _ in self.value_ratio)
        for t, a, c in opts:
            if t and not (0 <= a <= vmax and 0 <= c <= vmax):
                probs.append("数值 %d/%d 超出 0..%d" % (a, c, vmax))
        return probs


def roi_bytes_for(random_option_item):
    """登记拍卖时的 ROI_Category（12 字节）。普通装备全 0；随机属性装备 = 游戏端把空槽填 30000 后的结果
    （high[3]=30000, reserved=0, low[3]=0）。"""
    if not random_option_item:
        return b"\0" * 12
    return struct.pack("<hhhH", ROI_UNSEALED_KEY, ROI_UNSEALED_KEY, ROI_UNSEALED_KEY, 0) + b"\0" * 4
