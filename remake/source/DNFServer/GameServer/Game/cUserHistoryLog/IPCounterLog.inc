// ORIG _ZN15cUserHistoryLog12IPCounterLogEiiiiiPc @0x08685d0c
void cUserHistoryLog::IPCounterLog(int a, int b, int c, int d, int e, char* ip)
{
    m_user->LogHistory("IPCount,%d,%d,%d,%d,%d,%s", a, b, c, d, e, ip);
}