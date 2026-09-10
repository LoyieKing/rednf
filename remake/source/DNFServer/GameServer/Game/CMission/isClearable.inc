bool CMission_rank::isClearable(MissionInfo& info, bool&) const
{
    return m_clearCount <= (int)info.m_count;
