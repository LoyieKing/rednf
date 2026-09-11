// DungeonClearInfo(int, long)：ORIG 0x8684ac4，经 CUser::LogHistory 记录
void cUserHistoryLog::DungeonClearInfo(int isLast, long playTimeSec)
{
    m_user->LogHistory("DungeonClearInfo,%d,%d", isLast, playTimeSec);
}