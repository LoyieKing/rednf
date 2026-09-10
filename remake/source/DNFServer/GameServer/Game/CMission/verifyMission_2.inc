bool CMission_join::verifyMission(const MissionInfo& info) const
{
    if (m_joinCount + 10 <= (int)info.m_count)
        return false;
    return true;
