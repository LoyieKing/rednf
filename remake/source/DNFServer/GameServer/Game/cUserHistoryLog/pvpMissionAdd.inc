void cUserHistoryLog::pvpMissionAdd(int a, int b, int c, int d)
{
    m_user->LogHistory("PvPMission+,%d,%d,%d,%d", a, b, c, d);
}