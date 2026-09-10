bool CMission_winning_streak::isClearable(MissionInfo& info, bool&) const
{
    return m_clear <= (int)info.m_count;
