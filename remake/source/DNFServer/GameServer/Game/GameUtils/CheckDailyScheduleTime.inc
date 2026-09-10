bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime)
{
    time_t cur = lCurTime;
    struct tm tmv;
    localtime_r(&cur, &tmv);
    struct tm t2 = tmv;
    t2.tm_hour = nScheduleTime;
    t2.tm_min = 0;
    t2.tm_sec = 0;
    time_t t = mktime(&t2);
    if (tmv.tm_hour < nScheduleTime)
        t -= 86400;
    return lLastPlayTime <= t;
