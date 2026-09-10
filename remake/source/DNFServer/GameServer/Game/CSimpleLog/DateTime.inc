CSimpleLog::DateTime::DateTime()
{
    time_t t = time((time_t*)0);
    struct tm tm_buf;
    struct tm* p = localtime_r(&t, &tm_buf); // P43: 经返回值访问字段
    m_year  = p->tm_year + 0x76c;
    m_month = p->tm_mon + 1;
    m_day   = p->tm_mday;
    m_hour  = p->tm_hour;
    m_min   = p->tm_min;
    m_sec   = p->tm_sec;
