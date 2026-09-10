void WongWork::CSecurityCardCenter::_updateTimeCount()
{
    // ORIG T 0x085fe026 (size 0x38)：半字计数自增，越过 0xc34f 回绕为 1。
    ++m_timeCount;
    if (m_timeCount > 0xc34f)
    {
        m_timeCount = 1;
    }
