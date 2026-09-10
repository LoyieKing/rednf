void PacketBuf::set_packet(const char* data, int size)
{
    if (data == NULL || size < 1 || size > MAX_PACKET_SIZE)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x270,
                              "!packet || size<1 || size> MAX_PACKET_SIZE");
    }
    else
    {
        clear();
        memcpy(m_ptr, data, size);
        m_size = size;
    }
