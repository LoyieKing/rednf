void CSystemTime::update()
{
    gettimeofday(&m_tv, 0);
    m_sec = m_tv.tv_sec;
    m_msec = (m_sec - m_lastSec) * 1000 + m_tv.tv_usec / 1000;
