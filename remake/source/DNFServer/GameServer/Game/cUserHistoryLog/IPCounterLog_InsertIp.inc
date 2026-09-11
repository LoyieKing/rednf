// ORIG _ZN15cUserHistoryLog21IPCounterLog_InsertIpEiiPc @0x08685d9c
void cUserHistoryLog::IPCounterLog_InsertIp(int a, int b, char* ip)
{
    m_user->LogHistory("IPCountInt,%d,%d,%s", a, b, ip);
}