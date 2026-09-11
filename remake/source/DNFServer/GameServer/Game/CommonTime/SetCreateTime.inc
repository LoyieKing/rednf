void CommonTime::SetCreateTime(long t)
{
    tm tmv;
    tm* ptm = localtime_r(&t, &tmv);
    m_year  = (char)(ptm->tm_year - 100);
    m_month = (char)(ptm->tm_mon + 1);
    m_day   = (char)ptm->tm_mday;
    m_hour  = (char)ptm->tm_hour;
    m_min   = (char)ptm->tm_min;
    m_wday  = (char)ptm->tm_wday;
    m_curTime = t;
    m_createTime = t;
}