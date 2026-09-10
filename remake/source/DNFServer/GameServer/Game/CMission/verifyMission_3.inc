bool CMission_kill_count::verifyMission(const MissionInfo& info) const
{
    if (m_killCount + 10 <= (int)info.m_count)
        return false;
    return true;
