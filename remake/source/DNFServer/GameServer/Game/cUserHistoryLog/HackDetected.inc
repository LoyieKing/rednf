// ORIG _ZN15cUserHistoryLog11HackDetectedEjjjjjj @0x08685320：按尾参是否为零递增格式长度
void cUserHistoryLog::HackDetected(unsigned int a, unsigned int b, unsigned int c,
                                   unsigned int d, unsigned int e, unsigned int f)
{
    if (e == 0 && f == 0)
        m_user->LogHistory("Hack,%d,%d,%d,%d", a, b, c, d);
    else if (f == 0)
        m_user->LogHistory("Hack,%d,%d,%d,%d,%d", a, b, c, d, e);
    else
        m_user->LogHistory("Hack,%d,%d,%d,%d,%d,%d", a, b, c, d, e, f);
}