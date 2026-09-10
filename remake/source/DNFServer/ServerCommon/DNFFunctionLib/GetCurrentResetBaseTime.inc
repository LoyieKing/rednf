long GetCurrentResetBaseTime(long t, int hour) {
    long yester_time = t - 86400;
    tm now;
    tm yday;
    tm* tm_ptr = localtime_r(&t, &now);
    tm* tm_ptr_yester = localtime_r(&yester_time, &yday);
    // ORIG：localtime_r 调用后声明 reset_time（置 0）与 day（赋 hour）。
    long reset_time = 0;
    int day;
    day = hour;
    // 原始：条件为 tm_ptr->tm_hour >= 0 && tm_ptr->tm_hour < day 时改 ydayP，否则改 nowP
    if (tm_ptr->tm_hour >= 0 && tm_ptr->tm_hour < day) {
        tm_ptr_yester->tm_hour = day;
        tm_ptr_yester->tm_min = 0;
        tm_ptr_yester->tm_sec = 0;
        return mktime(tm_ptr_yester);
    } else {
        tm_ptr->tm_hour = day;
        tm_ptr->tm_min = 0;
        tm_ptr->tm_sec = 0;
        return mktime(tm_ptr);
    }
}
