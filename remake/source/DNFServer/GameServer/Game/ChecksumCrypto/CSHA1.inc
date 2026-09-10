CSHA1::CSHA1()
{
    // ORIG 0x08087d28：m_blockPtr = this+0x80; Reset();
    m_blockPtr = m_work;
    Reset();
