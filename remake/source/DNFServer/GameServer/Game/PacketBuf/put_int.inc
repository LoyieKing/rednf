bool PacketBuf::put_int(int& index, int value)
{
    if (is_ptr_end(index, 4))
    {
        return false;
    }

    switch (m_byte_encoding)
    {
    case 0:
        *(char*)(m_ptr + index++) = (char)value;
        *(char*)(m_ptr + index++) = (char)((value & 0xff00) >> 8);
        *(char*)(m_ptr + index++) = (char)((value & 0xff0000) >> 16);
        *(char*)(m_ptr + index++) = (char)((unsigned int)value >> 24);
        break;
    case 1:
        *(char*)(m_ptr + index++) = (char)((unsigned int)value >> 24);
        *(char*)(m_ptr + index++) = (char)((value & 0xff0000) >> 16);
        *(char*)(m_ptr + index++) = (char)((value & 0xff00) >> 8);
        *(char*)(m_ptr + index++) = (char)value;
        break;
    default:
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0xc4,
                              "m_byte_encoding error %d", m_byte_encoding);
        break;
    }
    return true;
}