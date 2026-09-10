void WindowsDataFile::GenerateChecksum(CodeHackChecksum* ck,
                                       unsigned long offset,
                                       unsigned long len)    // 0x08574eba
{
    if (m_size <= offset)
    {
        ZeroMemoryChecksum(ck);
        return;
    }
    if (m_size < offset + len)
    {
        len = m_size - offset;
    }
    CalculateMemoryChecksum(ck, len, m_data + offset);
