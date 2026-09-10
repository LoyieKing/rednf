bool CMission_revenge::isClearable(MissionInfo& info, bool&) const
{
    return m_revengeCount <= (int)info.m_count;
