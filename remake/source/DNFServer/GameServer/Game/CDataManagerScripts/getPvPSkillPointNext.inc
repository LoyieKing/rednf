// ============================================================================
// getPvPSkillPointNext（ORIG T 0x08a5df0e，const）：
// m_map18[grade] 与 m_map18[grade-1] 中 (job*5+firstGrow) 的投入点数差，
// 再减 m_map30[(job,firstGrow)] 的 m_pointCost。secondGrow 不参与
// （ORIG 写入 0 后仅用于 key 第三字节，恒 0）。
// ============================================================================
// [b28d-solo-CDataManagerScripts-2] 真 bug 修正：ORIG 0x08a5df66 `je +0xde` ⇒
// m_map18 无 grade 时直接 `mov $0,%eax; jmp end` **返回 0**（Ghidra：`if (==) local_10=0
// else { 全部计算 }`）；旧实现仍算 prevPoints/cost 并返回 `0-prevPoints-cost`（负值）。
// 带参 `PvPSkillTreeKey key(job,firstGrow,secondGrow)` + `secondGrow=0` 写回形参槽保留
// （ORIG C1Ehhh 0x08a5e768）。
// [b28d-solo-CDataManagerScripts-3] P220/P47 块序修正：ORIG 0x08a5df5f 调
// `_Rb_tree_const_iterator::ne`（operator!=）后 `test %al; je 0x08a5dfec`——return-0
// 块（`mov $0,%eax; jmp leave`）位于函数**中部**、主体直落尾；旧 `if (it == end) return 0;`
// 早退形在 4.4 下把 return-0 块排到主体之前（块序相反）。改正极包裹
// `if (gradeIt != m_map18.end()) { …; return …; } return 0;` 后布局与 ORIG 同向。
// 残余 gap 1 + 槽号整体平移 4（gradeIt -0x38 vs ORIG -0x34；P112/N93 块级槽向）；ORIG 0x08a5df0e。
// [b28d-solo-CDataManagerScripts-5 复核] fresh orig 183 / ours 182；jcc 30/30
// （序列错位源于块序/岛，无极性反写）⇒ 维持 N93/N112。
// [GW20Run 2026-09-11 复核] fresh 183/182；ORIG 0x08a5dfea 冗余 `jmp $+0xe8`
// （4.1 中继岛，我方 4.4 直跳）+ 尾 nop 豁免；p167 23/23（唯一差
// PvPSkillTreeKey C1/C2 同址别名）⇒ 维持。
int PvPSkillTreeParameterScript::getPvPSkillPointNext(
    int job, int firstGrow, int secondGrow, int grade) const
{
    int points = 0;
    std::map<int, std::map<int, int> >::const_iterator gradeIt =
        m_map18.find(grade);
    // [b28d-solo-CDataManagerScripts-3] P220/P47 正极包裹：ORIG 0x08a5df5f 调
    // operator!=（ne）后 `test %al; je <函数中部 mov$0; jmp leave>`，主体直落尾；
    // 旧 `== end` 早退形在 4.4 下把 return-0 块排到主体**之前**（块序相反）。
    if (gradeIt != m_map18.end())
    {
        {
            std::map<int, int>::const_iterator slotIt =
                gradeIt->second.find(job * 5 + firstGrow);
            if (slotIt != gradeIt->second.end())
                points = slotIt->second;
        }

        int prevPoints = 0;
        std::map<int, std::map<int, int> >::const_iterator prevIt =
            m_map18.find(grade - 1);
        if (prevIt != m_map18.end())
        {
            std::map<int, int>::const_iterator slotIt =
                prevIt->second.find(job * 5 + firstGrow);
            if (slotIt != prevIt->second.end())
                prevPoints = slotIt->second;
        }

        // ORIG 0x08a5e166：`movl $0x0,0x14(%ebp)`（secondGrow 写回形参槽）后
        // 三字节实参调 PvPSkillTreeKey::C1Ehhh（0x08a5e768）⇒ 带参构造形。
        secondGrow = 0;
        int cost = 0;
        PvPSkillTreeKey key(job, firstGrow, secondGrow);
        std::map<PvPSkillTreeKey,
                 FAIR_PVP_SKILL_TREE_SKILL_PER_POINT>::const_iterator costIt =
            m_map30.find(key);
        if (costIt != m_map30.end())
            cost = costIt->second.m_pointCost;

        return (points - prevPoints) - cost;
    }
    return 0;                       // ORIG 0x08a5dfec：grade 不存在 ⇒ 恒返 0
}