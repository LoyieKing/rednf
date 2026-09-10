int CBingo::getToday(long sec, int hourLimit)
{
    long yesterday = sec - 0x15180;   // 86400 秒 = 1 天
    struct tm tmToday;
    struct tm tmYest;
    struct tm* today = localtime_r(&sec, &tmToday);
    struct tm* yest = localtime_r(&yesterday, &tmYest);
    if (today->tm_hour >= 0 && hourLimit > today->tm_hour)
    {
        m_today = yest->tm_mday;
    }
    else
    {
        m_today = today->tm_mday;
    }
    return m_today;
