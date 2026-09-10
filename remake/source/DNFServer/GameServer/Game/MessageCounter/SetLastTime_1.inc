void CUnixTimeInterval::SetLastTime()
{
    gettimeofday(&m_last, 0);
