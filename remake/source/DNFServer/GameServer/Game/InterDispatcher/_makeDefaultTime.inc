time_t Inter_MonitorGuildPointDel::_makeDefaultTime()
{
    time_t now = time(0);
    struct tm localTm;
    struct tm* pTm = localtime_r(&now, &localTm);
    pTm->tm_mday = 1;
    pTm->tm_hour = 5;
    pTm->tm_min = 0;
    pTm->tm_sec = 0;
    if (pTm->tm_mon == 6 && pTm->tm_year == 0x6a)
        pTm->tm_mday = 6;
    now = mktime(pTm);
    return now;
