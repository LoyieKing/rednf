void cUserHistoryLog::pvpMissionClearReward(int expPoint, int exp, int missionKind, int missionIndex)
{
    m_user->LogHistory("PvPMissionReward+,%d,%d,%d,%d", expPoint, exp, missionKind, missionIndex);
}