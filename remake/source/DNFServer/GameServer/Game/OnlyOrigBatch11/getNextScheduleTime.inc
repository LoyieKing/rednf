// ORIG T 0x0846c34a：getPrevScheduleTime(hour, time + 0x15180)。
long getNextScheduleTime(int hour, long time)
{
    const int daySec = 0x15180;
    return getPrevScheduleTime(hour, time + daySec);
}