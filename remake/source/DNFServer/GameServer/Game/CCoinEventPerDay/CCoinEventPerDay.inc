// ---------------------------------------------------------------------------
// ORIG 0x0810ab10：基类构造 + vptr 置位 + 4 个档位字节（+0x09..+0x0c）置 1。
CCoinEventPerDay::CCoinEventPerDay()
{
    m_coin01 = 1;
    m_coin02 = 1;
    m_coin03 = 1;
    m_coin04 = 1;
}