void cUserHistoryLog::LeaveDungeon(const char* dungeonName, int unk, const char* memberNames, int state)
{
    m_user->LogHistory("DungeonLeave,\"%s\",%d,%d,%s", dungeonName, unk, state, memberNames);
