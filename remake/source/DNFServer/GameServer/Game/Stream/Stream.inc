Stream::Stream()
{
    // ORIG 存储顺序：+8(m_buffer) → +4(m_size) → +0xc → +0x10
    m_buffer = 0;
    m_size = 0;
    m_inPtr = 0;
    m_outPtr = 0;
    set_big_stream(false);
    resize(0x4000);
    m_poolType = 0;
