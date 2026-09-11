void cUserHistoryLog::LeaveDungeon(const char* dungeonName, int level)
{
    m_user->LogHistory("DungeonLeave,\"%s\",%d", dungeonName, level);
}