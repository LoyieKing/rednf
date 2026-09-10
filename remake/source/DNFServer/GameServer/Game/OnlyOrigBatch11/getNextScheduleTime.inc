long getNextScheduleTime(int hour, long time)
{
    const int daySec = 0x15180;
    return getPrevScheduleTime(hour, time + daySec);
