// ORIG T 0x086853e6（0x32 字节）：LogHistory("CleanPad,%d,%d", a, b)。
void cUserHistoryLog::RequestCleanPad(unsigned int a, char b)
{
    m_user->LogHistory("CleanPad,%d,%d", a, static_cast<int>(b));
}