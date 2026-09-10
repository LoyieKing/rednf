void CMission_winning_count::_makeTotalMissionInfo(MissionInfo& info,
                                                   const CUser& user) const
{
    // [b28c-35] NEAR/N7：ORIG 0x085e3548 虚调用段 `mov (%eax),%edx; …; call *%edx`
    // （间接 call 目标 %edx），4.4.7 恒 %eax；其余 28 条逐条全等。P230 补：N7 属
    // 4.4.6 后端敏感族，本波勿自切。
    if (m_prevMissionIndex != -1)
    {
        CMission* prev = (CMission*)G_CDataManager()->find_mission(
            m_prevMissionIndex);
        info.m_count = (short)prev->getClearCondition();
    }
