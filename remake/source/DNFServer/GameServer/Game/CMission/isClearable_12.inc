bool CMission_remains_hp::isClearable(MissionInfo& info, bool&) const
{
    if (m_clear <= (int)info.m_count)
        return true;
    return false;
