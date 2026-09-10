bool CMission_remain_hp::isClearable(MissionInfo& info, bool&) const
{
    return m_clear <= (int)info.m_count;
