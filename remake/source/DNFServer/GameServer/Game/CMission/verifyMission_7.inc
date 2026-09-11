bool CMission_revenge::verifyMission(const MissionInfo& info) const
{
    if (m_revengeCount + 10 <= (int)info.m_count)
        return false;
    return true;
}