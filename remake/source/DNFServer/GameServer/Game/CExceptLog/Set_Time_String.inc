void CExceptLog::Set_Time_String(char* buf)
{
    struct tm tm;
    time_t t;

    time(&t);
    localtime_r(&t, &tm);
    sprintf(buf, "%4d.%02d.%02d %02d:%02d:%02d",
            tm.tm_year + 0x76c, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);
