void cUserHistoryLog::DungeonClearInfo(int isLast, long playTimeSec)
{
    m_user->LogHistory("DungeonClearInfo,%d,%d", isLast, playTimeSec);
