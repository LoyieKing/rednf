void cUserHistoryLog::LeaveDungeon(int dungeonIdx, int unk, const char* memberNames, int state)
{
    m_user->LogHistory("DungeonLeave,%d,%d,%d,%s", dungeonIdx, unk, state, memberNames);
