void CBitManager::ShiftRight(int bits, unsigned char* buf)  // 0x0831c1d6
{
    for (int i = m_size; i > 0; --i)
    {
        buf[i - 1] = static_cast<unsigned char>(buf[i - 1] >> (bits & 0x1f));
        if ((i - 1) != 0)
        {
            buf[i - 1] = static_cast<unsigned char>(
                (static_cast<unsigned int>(buf[i - 2])
                     << ((8 - (bits & 0x1f)) & 0x1f)) |
                buf[i - 1]);
        }
    }
