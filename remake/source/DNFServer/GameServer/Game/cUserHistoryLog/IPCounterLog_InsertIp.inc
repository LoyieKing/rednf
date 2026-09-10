void cUserHistoryLog::IPCounterLog_InsertIp(int a, int b, char* ip)
{
    m_user->LogHistory("IPCountInt,%d,%d,%s", a, b, ip);
