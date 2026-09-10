void cUserHistoryLog::EnterDungeon(const char* dungeonName, int unk,
                                   const char* memberNames, int state)
{
    m_user->LogHistory("DungeonEnter,\"%s\",%d,%d,%s",
                       dungeonName, unk, state, memberNames);
