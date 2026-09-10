void CClientCrashDownLogStatistic::RegisterClientCrashDown(int reason)
{
    int v = m_reasonCount[reason];
    m_reasonCount[reason] = ++v;
