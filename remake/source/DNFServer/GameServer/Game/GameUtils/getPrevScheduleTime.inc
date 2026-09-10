long getPrevScheduleTime(int hour, long time)
{
    const int oneHour = 3600;
    long tmp = (long)hour * oneHour;
    long t = time - tmp;
    struct tm tmv;
    localtime_r(&t, &tmv);
    tmv.tm_hour = hour;
    tmv.tm_min = 0;
    tmv.tm_sec = 0;
    t = mktime(&tmv);
    return t;
