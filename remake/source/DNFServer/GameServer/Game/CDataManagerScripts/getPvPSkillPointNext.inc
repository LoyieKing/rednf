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
