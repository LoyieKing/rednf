// ORIG _ZN15cUserHistoryLog16ItemLockUsingLogEhiiii @0x08689808（格式串 ORIG 0x8cf1153）
void cUserHistoryLog::ItemLockUsingLog(unsigned char lockType, int a, int b, int c, int d)
{
    m_user->LogHistory("ItemLock,%d,%d,%d,%d,%d", lockType & 0xff, a, b, c, d);
}