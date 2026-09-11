// ORIG _ZN15cUserHistoryLog16FullIPCounterLogEiiiiiPc @0x08685d54
void cUserHistoryLog::FullIPCounterLog(int a, int b, int c, int d, int e, char* ip)
{
    m_user->LogHistory("D_IPCount,%d,%d,%d,%d,%d,%s", a, b, c, d, e, ip);
}