bool CMission_kill_count::isClearable(MissionInfo& info, bool&) const
{
    return m_killCount <= (int)info.m_count;
