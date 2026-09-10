void cUserHistoryLog::EnterDungeon(const char* dungeonName, int level)
{
    m_user->LogHistory("DungeonEnter,\"%s\",%d", dungeonName, level);
