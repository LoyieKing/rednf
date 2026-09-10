bool PacketBuf::get_short(short& out)
{
    if (is_len_end(2))
    {
        return false;
    }

    unsigned int b0 = (unsigned char)m_ptr[m_use_size++];
    unsigned int b1 = (unsigned char)m_ptr[m_use_size++];

    switch (m_byte_encoding)
    {
    case 0:
        out = (short)(b1 * 0x100 + b0);
        break;
    case 1:
        out = (short)(b0 * 0x100 + b1);
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x191,
                              "m_byte_encoding error %d", m_byte_encoding);
        return false;
    }
    return true;
