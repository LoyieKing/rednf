void CLog::getTime(TimeLog_* t)
{
    std::time_t now = std::time(0);
    std::tm lt;
    struct tm* p = localtime_r(&now, &lt);
    t->m_year = (short)(p->tm_year + 0x76c);
    t->m_mon = (unsigned char)((char)p->tm_mon + 1);
    t->m_day = (unsigned char)p->tm_mday;
    t->m_hour = (unsigned char)p->tm_hour;
    t->m_min = (unsigned char)p->tm_min;
    t->m_sec = (unsigned char)p->tm_sec;
