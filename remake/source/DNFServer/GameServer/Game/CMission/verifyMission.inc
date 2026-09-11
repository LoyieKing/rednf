bool CMission_winning_count::verifyMission(const MissionInfo& info) const
{
    if (m_winCount + 10 <= (int)info.m_count)
        return false;
    return true;
}