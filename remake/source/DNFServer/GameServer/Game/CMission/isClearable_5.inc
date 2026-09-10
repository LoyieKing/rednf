bool CMission_hereafter_join::isClearable(MissionInfo& info, bool&) const
{
    return m_joinCount <= (int)info.m_count;
