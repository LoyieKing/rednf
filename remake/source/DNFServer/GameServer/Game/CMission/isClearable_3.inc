bool CMission_join::isClearable(MissionInfo& info, bool&) const
{
    return m_joinCount <= (int)info.m_count;
