int PvPSkillTreeParameterScript::getPvPSkillPoint(
    int job, int firstGrow, int secondGrow, int grade, bool flag) const
{
    // ORIG 0x08a5dd62：grade 下限钳制到 m_field4c；
    // m_map18[grade][job*5+firstGrow] 为已投入点数；
    // m_map30[(job,firstGrow)] 的 m_pointCost(+0x40)：flag=true 且已有点数则扣减，
    // flag=false 且点数为 0 则取消耗值。secondGrow 在 ORIG 中被强制置 0（未参与）。
    // [b28d-56 判词·P167/P212] ORIG 0x08a5dd62 用三参 ctor `PvPSkillTreeKey(h,h,h)`
    // （_ZN15PvPSkillTreeKeyC1Ehhh，ORIG W 0x08a5e768），第三实参为立即数 0（ORIG
    // 无 secondGrow 形参槽写）；且 `movl $0x0,-0xc`（points）在**函数头**（P212 全
    // 头部声明）。旧码默认 ctor + 逐字段赋值 ⇒ p167 MISSING 三参 ctor/EXTRA 默认 ctor。
    // [b28d-56 判词·N] 残余 N82：ORIG 131 条 vs 我们 131 条（key 三参 ctor 与
    // points 头声明已对齐；残余为 grade 钳制分支布局 `jge` vs `jle`（P59/P222 极性）
    // 与 map 迭代器槽 -0x28 vs -0x18）。语义完成。
    int points = 0;
    if (grade < m_field4c)
        grade = m_field4c;

    std::map<int, std::map<int, int> >::const_iterator gradeIt = m_map18.find(grade);
    if (gradeIt != m_map18.end())
    {
        std::map<int, int>::const_iterator slotIt =
            gradeIt->second.find(job * 5 + firstGrow);
        if (slotIt != gradeIt->second.end())
            points += slotIt->second;
    }

    PvPSkillTreeKey key(static_cast<unsigned char>(job),
                        static_cast<unsigned char>(firstGrow), 0);
    std::map<PvPSkillTreeKey,
             FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::const_iterator costIt =
        m_map30.find(key);
    if (costIt != m_map30.end())
    {
        if (flag)
        {
            if (points != 0)
                points -= costIt->second.m_pointCost;
        }
        else if (points == 0)
        {
            points = costIt->second.m_pointCost;
        }
    }
    return points;
