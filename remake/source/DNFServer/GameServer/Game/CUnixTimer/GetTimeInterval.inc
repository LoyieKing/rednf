double CUnixTimer::GetTimeInterval()
{
    return GetNowTime() - m_lastTime;
}