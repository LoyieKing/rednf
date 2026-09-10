bool CheckDayScheduleTimeOver(int day, long checkTime)
{
    time_t now;
    time(&now);
    return now + static_cast<long>(day) * (-86400L) > checkTime;
