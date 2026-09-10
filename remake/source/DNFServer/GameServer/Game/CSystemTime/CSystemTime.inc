CSystemTime::CSystemTime()
{
    gettimeofday(&m_tv, 0);
    m_lastSec = m_tv.tv_sec;
    m_msec = m_tv.tv_usec / 1000;
