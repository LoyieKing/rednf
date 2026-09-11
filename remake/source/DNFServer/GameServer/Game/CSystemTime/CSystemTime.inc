// 构造：仅记录 m_lastSec 与毫秒（m_sec 留待 update 首次填充）
CSystemTime::CSystemTime()
{
    gettimeofday(&m_tv, 0);
    m_lastSec = m_tv.tv_sec;
    m_msec = m_tv.tv_usec / 1000;
}