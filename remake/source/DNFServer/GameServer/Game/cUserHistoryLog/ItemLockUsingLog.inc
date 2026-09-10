void cUserHistoryLog::ItemLockUsingLog(unsigned char lockType, int a, int b, int c, int d)
{
    m_user->LogHistory("ItemLock,%d,%d,%d,%d,%d", lockType & 0xff, a, b, c, d);
