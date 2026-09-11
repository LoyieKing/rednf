int CMission_remain_hp::Update(MissionInfo& info,
                               const MissionClearCondition_Parameter&) const
{
    ++info.m_count;
    return 1;
}