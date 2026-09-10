bool CMission_hereafter_winning_count::isClearable(MissionInfo& info,
                                                   bool&) const
{
    return m_winCount <= (int)info.m_count;
