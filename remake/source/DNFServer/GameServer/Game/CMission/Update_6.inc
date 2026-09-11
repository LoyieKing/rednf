int CMission_hereafter_winning_count::Update(
    MissionInfo& info, const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
    return 1;
}