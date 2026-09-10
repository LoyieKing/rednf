void TraceLog::get_time(char* todaystr, char* timestr)
{
    // 原始：time(&tval) 非 tval=time(NULL)
    time_t tval;
    time(&tval);
    tm* t = localtime(&tval);
    if (todaystr != NULL)
    {
        sprintf(todaystr, "%04d%02d%02d", t->tm_year + 0x76c, t->tm_mon + 1, t->tm_mday);
    }
    if (timestr != NULL)
    {
        sprintf(timestr, "%02d%02d%02d", t->tm_hour, t->tm_min, t->tm_sec);
    }
