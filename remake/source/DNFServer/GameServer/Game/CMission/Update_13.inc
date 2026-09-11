int CMission_move_channel::Update(
    MissionInfo& info, const MissionClearCondition_Parameter& param) const
{
    info.m_count = (short)param.m_channelTo;
    info.m_streak = (short)param.m_channelFrom;
    return 1;
}