// ---- _RandomLevel —— T 0x085f93cc（报告缺失，objdump 直转）----------------
void RegenerationROI::_RandomLevel(int lv0, int lv1, select_base_item& sb,
                                   std::stringstream& ss)
{
    // [b28d-solo-RandomRentalA2 判词] AE（[Lane13 2026-09-10] 已归零；原 DIFF 202/204；ORIG 0x085f93cc）：残余槽序/
    // 物化次数（N82 族）。旧判词：
    // 语义等价。
    ss << "try regen:lv:" << lv0 << "," << lv1 << "\n";

    if (lv0 > lv1)
    {
        std::swap(lv0, lv1);
    }

    // [Lane13] P28/P212：ORIG 085f943d `lea -0x24(%ebp),%eax; call vector C1` 的
    // levels 构造点位于 swap 之后、lo/hi 计算**之前**（O[33:35]）⇒ 声明须前置。
    std::vector<int> levels;

    // [SweepRandomRental 修正] ORIG 0x085f946f..0x085f94a6：`test;je → mov $0x0,%eax`
    // 与 `mov $0x5,%eax; sub %edx,%eax` 两臂**汇合后**再 `add lv0`（即 +0 走 add 形）
    // ⇒ 原码为 `lv0 + (cond ? 0 : 5 - lv0%5)`，非三目直接返回 lv0。
    int lo = lv0 + ((lv0 % 5 == 0) ? 0 : (5 - lv0 % 5));   // 向上取整到 5 的倍数
    int hi = lv1 - (lv1 % 5);                               // 向下取整到 5 的倍数

    for (int v = lo; v <= hi; v += 5)
    {
        levels.push_back(v);
    }

    if (levels.empty())
    {
        ss << "level count : Empty\n";
        return;
    }

    // [Lane13] P0/P236：ORIG 085f94xx `mov %eax,-0x18; lea -0x18(%ebp),%edx`
    // —— bound 落**浅**槽（紧贴 randInt 实参区），ours 的具名局部被放深 -0x24
    // 并顶浅 levels；改内联实参（randInt 取 const&，4.4 自建匿名临时）。
    int pick = m_regeneratorPtr->randInt((unsigned long)(levels.size() - 1));
    sb.m_level0 = levels[pick];

    if (sb.m_level0 % 10 != 0 && sb.m_level0 > 10)
    {
        sb.m_level0 += 5;
    }

    ss << "Level count : " << (unsigned int)levels.size()
       << " and select lv : " << sb.m_level0 << "\n";
}