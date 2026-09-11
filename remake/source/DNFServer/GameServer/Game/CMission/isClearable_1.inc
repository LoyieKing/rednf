bool CMission_winning_count::isClearable(MissionInfo& info, bool&) const
{
    return m_winCount <= (int)info.m_count;
}