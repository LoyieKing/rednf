Stream::Stream(int size)
{
    m_buffer = 0;
    m_size = 0;
    m_inPtr = 0;
    m_outPtr = 0;
    set_big_stream(false);
    resize(size);
    m_poolType = 0;
}