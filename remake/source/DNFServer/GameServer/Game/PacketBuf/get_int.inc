bool PacketBuf::get_int(int& out)
{
    if (is_len_end(4))
    {
        return false;
    }

    int b0 = (unsigned char)m_ptr[m_use_size++];
    int b1 = (unsigned char)m_ptr[m_use_size++];
    int b2 = (unsigned char)m_ptr[m_use_size++];
    int b3 = (unsigned char)m_ptr[m_use_size++];

    switch (m_byte_encoding)
    {
    case 0:
        out = (b3 << 24) + (b2 << 16) + (b1 << 8) + b0;
        break;
    case 1:
        out = (b0 << 24) + (b1 << 16) + (b2 << 8) + b3;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x1b7,
                              "m_byte_encoding error %d", m_byte_encoding);
        return false;
    }
    return true;
}