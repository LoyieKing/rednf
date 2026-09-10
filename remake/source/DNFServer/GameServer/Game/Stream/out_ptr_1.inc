int Stream::out_ptr(int n)
{
    if (m_outPtr + n > m_inPtr)
    {
        return -1;
    }
    m_outPtr += n;
    if (m_outPtr == m_inPtr)
    {
        reset();
    }
    return m_outPtr;
