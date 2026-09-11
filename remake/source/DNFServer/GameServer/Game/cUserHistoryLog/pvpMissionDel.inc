void cUserHistoryLog::pvpMissionDel(int a, int b)
{
    m_user->LogHistory("PvPMission-,%d,%d", a, b);
}