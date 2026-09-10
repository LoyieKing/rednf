void cUserHistoryLog::DungeonClearInfo(int isLast, long playTimeSec, int idx, int mode2,
                                       int val2, ENUM_DUNGEON_MODE dungeonMode,
                                       const char* name, int val3)
{
    m_user->LogHistory("AdvanceDungeonClearInfo,%d,%d,%d,%d,%d,%d,%s,%d",
                       idx, mode2, isLast, playTimeSec, val2, static_cast<int>(dungeonMode),
                       name, val3);
