char checkTimeToday(long t, int hour) {
    long yesterday = t - 86400;
    tm now;
    tm yday;
    tm* nowP = localtime_r(&t, &now);
    tm* ydayP = localtime_r(&yesterday, &yday);
    (void)ydayP;
    int localHour = hour;
    char result = 0;
    if (nowP->tm_hour >= 0 && nowP->tm_hour < localHour) {
        result = 0;
    } else {
        result = 1;
    }
    return result;
