int CMission_kill_count::Update(MissionInfo& info,
                                const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
    return 1;
}