void CommonTime::MakeTime(tm& out) const
{
    out.tm_sec = 0;
    out.tm_min = m_min;
    out.tm_hour = m_hour;
    out.tm_mday = m_day;
    out.tm_mon = m_month - 1;
    out.tm_year = m_year + 100;
    out.tm_isdst = 0;
