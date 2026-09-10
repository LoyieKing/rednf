int ComputeDaysScheduleTime(int hour, long lastTime, long curTime)
{
    time_t c = curTime;
    struct tm tmc;
    localtime_r(&c, &tmc);
    struct tm t1 = tmc;
    t1.tm_hour = hour;
    t1.tm_min = 0;
    t1.tm_sec = 0;
    time_t r1 = mktime(&t1);
    if (tmc.tm_hour < hour)
        r1 -= 86400;

    time_t l = lastTime;
    struct tm tml;
    localtime_r(&l, &tml);
    struct tm t2 = tml;
    t2.tm_hour = hour;
    t2.tm_min = 0;
    t2.tm_sec = 0;
    time_t r2 = mktime(&t2);
    if (tml.tm_hour < hour)
        r2 -= 86400;

    int days = (int)(r1 - r2) / 86400;
    if (days < 0)
        days = 0;
    return days;
