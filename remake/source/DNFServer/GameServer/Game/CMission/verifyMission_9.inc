bool CMission_kill::verifyMission(const MissionInfo& info) const
{
    if (m_clear + 10 <= (int)info.m_count)
        return false;
    return true;
