// ---- _get_option_type —— T 0x085f2338（同名报告全文转写）-----------------
// 经 script+0x148 provider 虚槽0 取 map<RAKey, map<int,RandomOptionGain>>；
// 内层先 find(-1)，未命中再 find(group)。
// 【运行期 blocked】provider 由 importRandomOptionScript 批次构造。
int CRandomOptionItemHandle::_get_option_type(unsigned long /*itemIdx*/,
                                              ENUM_RARITY rarity, int level,
                                              int group, unsigned char& outType)
{
    // [Lane17 G 判词] NEAR 184/185（ORIG 0x085f2338）：判据⑪ 直方图**逐值全等**
    // {−0xc…−0x6c 全部 20 值两侧一致）⇒ 非整帧平移、非缺槽，而是**同槽集的相对
    // 序差**（defaultKey/key/outer/g/give 五者的槽分配互换：ORIG key=−0x58、
    // defaultKey=−0x5c、outer=−0x4c、g=−0x50 vs 我方 −0x50/−0x48/−0x48/−0x4c）。
    // 判据⑩ 声明重排本可攻（成本 1–2 次编译）；本轮因编辑命中错位已回退，
    // 记「待攻」而非 N —— 属 (a) 类相对序差。旧判词：DIFF 184/176。
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);

    // [b28d-solo-RandomRentalA2] P0/P183：ORIG 每处 map 操作都重取 m_optionSet
    // 并虚调 get_option_type()（find/end 各一次，无 provider/giveMap 具名局部）。
    // CRandomOptionGive 纯虚 ORIG 名（UserMiscLink 批改名）
    // [Lane13 试形回滚] 「先声明后赋值」形实测反劣：near→diff 且 missing_in_game
    // 96→98（多出迭代器 C1/C2 弱符号，P50 在本件不适用）⇒ 维持初始化式直构。
    std::map<RandomApplicationKey,
             std::map<int, RandomOptionGive> >::const_iterator outer =
        static_cast<CRandomOptionGive*>(m_optionSet)->get_option_type().find(key);
    if (outer ==
        static_cast<CRandomOptionGive*>(m_optionSet)->get_option_type().end())
    {
        return 0x23f;
    }

    // [SweepRandomRental-2] ORIG 0x085f23f9 `movl $0x0,-0x10(%ebp)`（give 指针
    // 先置 0）+ 0x085f2400 `movl $0xffffffff,-0x48`（defaultKey）；命中/回退两
    // 分支各自 `call ptEv; add $0x4; mov %eax,-0x10`（0x085f2472 / 0x085f24eb）
    // ⇒ 原码是**指针局部**（引用形只在 if 之后单点赋值，块布局不同）。
    // [Lane13] P309/P331 槽位旋钮：ORIG defaultKey 落 -0x48（浅）、g 落 -0x60（深）；
    // 4.4 对具名标量按「先声明更浅」⇒ 把 defaultKey 声明提前到 give 之前。
    // [Lane17 G2 已试并回退] 把 defaultKey 再提到 key/outer 之前：实测**反劣**
    // （NEAR→DIFF，TU four 295/335/1/6 → 295/335/0/7；槽集由 {−0x50,−0x52,−0x58}
    // 变 {−0x54,−0x56,−0x5c}）⇒ 维持现序，按 (a) 类相对序差记「待攻」。
    int defaultKey = -1;
    // [PoolB b28f · PoolE 旋钮实验·负结论] 曾把指针局部 `give` 改 `unsigned int`
    // 承载地址（PoolE 分区旋钮）⇒ 实测**反劣**（NEAR 184/185 → DIFF 186/184、
    // ndiff 增），已回退。本件残余维持判词：同槽集相对序差（defaultKey/key/
    // outer/g/give 五者槽互换），非指针分区所致。
    const RandomOptionGive* give = 0;
    std::map<int, RandomOptionGive>::const_iterator g =
        outer->second.find(defaultKey);
    if (g != outer->second.end())
    {
        give = &g->second;
    }
    else
    {
        g = outer->second.find(group);
        // [SweepRandomRental-2] ORIG 0x085f24d9 `je 085f24f0`（命中直落 give
        // 赋值、失败落 `mov $0x255` 尾块）⇒ if/else 正极性（早返形把失败块
        // 提到前面、多 2 条 jmp）。
        if (g != outer->second.end())
        {
            give = &g->second;
        }
        else
        {
            return 0x255;
        }
    }

    // [b28d-solo-RandomRentalA2 P0] bound 匿名临时形。
    int r = m_rand[4].randInt((unsigned long)give->m_c);

    // [Lane13] P278/P221：ORIG 085f2450 把 CRandomList 构到 **-0x28**（浅槽），
    // get_value 作用在 for_each 返回值匿名 sret（-0x38）上 ⇒ 全表达式链；具名
    // chooser 被 4.4 放最深 -0x60 并整体顶浅其余槽。
    outType = (unsigned char)(
        std::for_each(give->m_vec0.begin(), give->m_vec0.end(),
                      CRandomList<unsigned int>((unsigned int)r))
            .get_value() +
        1);
    return 0;
}