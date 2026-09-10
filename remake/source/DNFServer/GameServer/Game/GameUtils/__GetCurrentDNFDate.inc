void __GetCurrentDNFDate(tm& out)
{
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    int daily = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // +0x37c
    localtime_r(&cur, &out);
    if (out.tm_hour < daily)
    {
        cur -= 86400;
        localtime_r(&cur, &out);
    }
