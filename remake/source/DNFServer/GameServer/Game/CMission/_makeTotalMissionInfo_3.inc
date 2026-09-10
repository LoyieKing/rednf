void CMission_join::_makeTotalMissionInfo(MissionInfo& info,
                                          const CUser&) const
{
    // [b28c-35] NEAR/N7：ORIG 0x085e378a 虚调用 `mov (%eax),%edx; mov -0xc,%eax;
    // call *%edx`（目标 %edx），4.4.7 恒 %eax；其余 28 条全等（含 P0 具名局部
    // prev 的 -0xc 槽往返）。4.4.6 后端可翻，本波勿自切。
    if (m_prevMissionIndex != -1)
    {
        CMission* prev = (CMission*)G_CDataManager()->find_mission(
            m_prevMissionIndex);
        info.m_count = (short)prev->getClearCondition();
    }
