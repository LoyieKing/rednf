void CMission_kill_count::_makeTotalMissionInfo(MissionInfo& info,
                                                const CUser&) const
{
    // [b28c-35] NEAR/N7：同 join（ORIG 0x085e38xx 虚调用目标 %edx vs 4.4.7 %eax）。
    if (m_prevMissionIndex != -1)
    {
        CMission* prev = (CMission*)G_CDataManager()->find_mission(
            m_prevMissionIndex);
        info.m_count = (short)prev->getClearCondition();
    }
