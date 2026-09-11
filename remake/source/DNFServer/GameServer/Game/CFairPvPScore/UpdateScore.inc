// ---- ORIG 0x84a5a28 UpdateScore(int,int,int,int,int,int,int) ---------------
// b28d-solo-CFairPvPScore 判词：AE/IDENTICAL（已转出 TU DIFF 列表；211=211，
// 归一化 NDIFF 仅剩 1 条尾部对齐 nop = §6.1 豁免）。
// 修法：①**真语义 bug**——新键插入值应为本次 cnt0/cnt1/cnt2（ORIG 084a5ae8..
// 084a5af7 从 0x18/0x1c/0x20(%ebp) 取），旧转写误插 0（丢失首局计数）；
// ②P220 正极性 `it != end()` + then/else 交换（p167 差集 ne/eq 实证）；
// ③P178 嵌套实参形：`pair<const JobKey,COUNT>(make_pair(key, value))` 复现
// make_pair<JobKey&,COUNT&> + pair 移动构造两段；④FAIR_PVP_SCORE_MODE_COUNT 与
// m_modeStats 字段 unsigned 化（P274：unsigned 字段 + int 形参 ⇒ ORIG 084a5b4a
// `mov mem,%edx; mov param,%eax; add %eax,%edx` 三段形）。p167 CLEAR（16=16）。
void CFairPvPScore::UpdateScore(int jobId, int subKey, int mode,
                                int cnt0, int cnt1, int cnt2, int resultKind)
{
    if (mode == 1)
    {
        JobKey key((ENUM_CHARACTERJOB)jobId, (unsigned char)subKey);
        std::map<JobKey, FAIR_PVP_SCORE_MODE_COUNT>::iterator it =
            m_jobScore_.find(key);
        if (it != m_jobScore_.end())
        {
            it->second.m_cnt0 += cnt0;
            it->second.m_cnt1 += cnt1;
            it->second.m_cnt2 += cnt2;
        }
        else
        {
            // ORIG 084a5ae8..084a5b44：新键插入值 = 本次 cnt0/cnt1/cnt2
            // （旧转写误插 0，属真语义 bug）；插入走 make_pair 临时 + pair
            // 移动构造两段（P178 嵌套实参形，p167 差集实证）。
            FAIR_PVP_SCORE_MODE_COUNT value;
            value.m_cnt0 = cnt0;
            value.m_cnt1 = cnt1;
            value.m_cnt2 = cnt2;
            m_jobScore_.insert(
                std::pair<const JobKey, FAIR_PVP_SCORE_MODE_COUNT>(
                    std::make_pair(key, value)));
        }
        m_modeStats[0][0] += cnt0;
        m_modeStats[0][1] += cnt1;
        m_modeStats[0][2] += cnt2;
    }
    else
    {
        if (resultKind == 2)
            ++m_field40;
        else if (resultKind == 3)
            ++m_field44;
        m_modeStats[1][0] += cnt0;
        m_modeStats[1][1] += cnt1;
        m_modeStats[1][2] += cnt2;
    }

    if (cnt0 != 0)
        m_recentResults_.push_back(1);
    else if (cnt1 != 0)
        m_recentResults_.push_back(2);
    else if (cnt2 != 0)
        m_recentResults_.push_back(3);

    if (m_recentResults_.size() > 10)
        m_recentResults_.pop_front();

    if (cnt1 != 0)
    {
        m_winStreak = 0;
    }
    else if (cnt0 != 0)
    {
        ++m_winStreak;
        if (m_bestWinStreak < m_winStreak)
            m_bestWinStreak = m_winStreak;
    }

    ++m_dailyPlayCount;
    alter();
}