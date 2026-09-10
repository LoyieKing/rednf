void cUserHistoryLog::MapClear(unsigned short itemIdx, int playSec, int a)
{
    m_user->LogHistory("MapClear,%d,%d,%d", (unsigned)itemIdx, playSec, a);
