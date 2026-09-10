void Stream::reset()
{
    m_inPtr = 0;
    m_outPtr = 0;
    set_big_stream(false);
    m_poolType = 0;
