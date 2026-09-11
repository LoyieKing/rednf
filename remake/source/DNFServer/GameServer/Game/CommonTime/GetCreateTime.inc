void CommonTime::GetCreateTime(char* buf) const
{
    // ORIG 秒位固定输出 0
    sprintf(buf, "%4d-%2d-%2d %2d:%2d:%2d", (int)m_year + 2000, (int)m_month,
            (int)m_day, (int)m_hour, (int)m_min, 0);
}