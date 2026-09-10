int Stream::resize(int n)
{
    char* buf = new char[n];
    if (!buf)
    {
        return 0;
    }
    memcpy(buf, m_buffer + out_ptr(), length());
    if (m_buffer)
    {
        delete[] m_buffer;
    }
    m_outPtr = 0;
    m_inPtr = length();
    m_buffer = buf;
    m_size = n;
    return 1;
