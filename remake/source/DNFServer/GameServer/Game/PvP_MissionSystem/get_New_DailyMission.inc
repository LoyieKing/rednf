void PvP_MissionSystem::get_New_DailyMission(
    std::vector<std::pair<unsigned int, unsigned int> >& out)
{
    // ORIG 0x85e26dc 逐指令转写：三段——①dailyMissionIndices→map 查 mission
    // 推 {kind,idx}；②按 m_dailyMissionList 加权随机补 m_field30 个；③bitset
    // 校验 kind 范围/重复，越界或重复打印 cMyTrace 并清空 out。
    out.clear();
    for (std::vector<unsigned int>::size_type i = 0;
         i < m_dailyMissionIndices.size(); ++i)
    {
        const unsigned int id = m_dailyMissionIndices[i];
        std::map<int, CMission*>::iterator it =
            m_missionMap.find(static_cast<int>(id));
        if (it == m_missionMap.end())
            continue;
        CMission* mission = it->second;
        if (mission == 0)
            continue;
        // [R26D] P167：ORIG 用 `_ZSt9make_pairIiiE`（int,int），旧文的多余
        // static_cast<unsigned int> 使其选 `make_pairIjjE`（MISSING i 版 ×2 /
        // EXTRA j 版 ×2）；push_back 处由 pair<unsigned,unsigned> 转换构造承接。
        out.push_back(std::make_pair(
            mission->GetMissionKind(),
            mission->GetMissionIndex()));
    }
    // [R26D · 轴 B 反向命中] ORIG 0x085e2ac0 段 `cmp -0x34(%ebp),%eax; seta %al`
    // （无符号序）⇒ 循环变量须为 unsigned；旧文 `int j` 使 4.4 产 `setg`
    // （有符号，/tmp/r5/axisB_points.tsv 记 seta→setg）。`j < m_field30`（int 成员）
    // 经同宽无指令转换即无符号比较，P196 前置已核 ORIG 为 seta ⇒ 允许改。
    for (unsigned int j = 0; j < (unsigned int)m_field30; ++j)
    {
        std::list<std::pair<unsigned int, unsigned int> > tmp(m_dailyMissionList);
        int weight = m_field3c;
        int r = static_cast<CMTRand*>(m_trand)->randInt() % weight;
        int sum = 0;
        for (std::list<std::pair<unsigned int, unsigned int> >::iterator li =
                 tmp.begin(); li != tmp.end(); ++li)
        {
            sum += static_cast<int>(li->first);
            if (r < sum)
            {
                const unsigned int missionIdx = li->second;
                std::map<int, CMission*>::iterator mi =
                    m_missionMap.find(static_cast<int>(missionIdx));
                // [R26D] P167：ORIG 该点用 `==`（`_ZNK...eqERKS5_` ×1 差集）⇒
                // 写取反式而非 `!=`（ne 版是 EXTRA）。
                if (!(mi == m_missionMap.end()) && mi->second != 0)
                {
                    CMission* m = (*mi).second;
                    out.push_back(std::make_pair(
                        m->GetMissionKind(),
                        m->GetMissionIndex()));
                }
                weight -= static_cast<int>(li->first);
                tmp.erase(li);
                break;
            }
        }
        if (weight <= 0)
        {
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x123, 5)
                ("InvalidMaxDice %d", weight);
        }
    }
    std::bitset<32> dailyKind(0);
    for (std::vector<std::pair<unsigned int, unsigned int> >::size_type k = 0;
         k < out.size(); ++k)
    {
        const unsigned int kind = out[k].first;
        if (kind == 0)
            continue;
        if (kind > 0x20)
        {
            std::string s;
            for (std::vector<std::pair<unsigned int, unsigned int> >::size_type
                     m = 0; m < out.size(); ++m)
            {
                char buf[0x110];
                sprintf(buf, "<K:%d, I:%d>", out[m].first, out[m].second);
                s += buf;
            }
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x139, 5)
                ("[@missionSystem] OverKind: <Invalid mission kind : %d> "
                 "<mission index : %d> <Created DailyMission Size : %d> "
                 "<List : %s >",
                 kind, out[k].second, out.size(), s.c_str());
            out.clear();
        }
        else if (dailyKind[kind])
        {
            std::string s;
            for (std::vector<std::pair<unsigned int, unsigned int> >::size_type
                     m = 0; m < out.size(); ++m)
            {
                char buf[0x110];
                sprintf(buf, "<K:%d, I:%d>", out[m].first, out[m].second);
                s += buf;
            }
            cMyTrace("void PvP_MissionSystem::get_New_DailyMission", 0x14d, 5)
                ("[@missionSystem] OverlappedKind: <Invalid mission kind : %d> "
                 "<mission index : %d> <Created DailyMission Size : %d> "
                 "<List : %s >",
                 kind, out[k].second, out.size(), s.c_str());
            out.clear();
        }
        else
        {
            dailyKind[kind] = true;
        }
    }
