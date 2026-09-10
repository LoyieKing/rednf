int getDailyInitializeTime(int hour, int min, int sec)
{
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm tmv;
    struct tm* p = localtime_r(&cur, &tmv);
    p->tm_hour = hour;
    p->tm_min = min;
    p->tm_sec = sec;
    time_t ret = mktime(p);
    if (ret > cur)
        ret -= 86400;
    return (int)ret;
