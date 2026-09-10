CCoinRefillEvent::CCoinRefillEvent()
{
    // ORIG 0x0832bdf4：vptr 由编译器写；memset(this+0xc, -1, 0x14)。
    memset(&m_timeCount, -1, 0x14);
