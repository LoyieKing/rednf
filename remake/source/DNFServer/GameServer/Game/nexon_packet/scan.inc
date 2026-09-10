bool CPacketHelper::scan(unsigned char* dst, int count)
{
    if (static_cast<int>(static_cast<unsigned>(m_offset)) + count >= m_size)
        return false;
    for (int i = 0; i < count; ++i)
    {
        dst[i] = static_cast<char>(m_buffer[m_offset]);
        ++m_offset;
    }
    return true;
