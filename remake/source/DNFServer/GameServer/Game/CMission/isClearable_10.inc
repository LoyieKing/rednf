bool CMission_kill::isClearable(MissionInfo& info, bool&) const
{
    return m_clear <= (int)info.m_count;
}