bool PacketBuf::bind_packet(char* packet, int size)
{
    if (packet == NULL || size == 0)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x27c,
                              "packet is null or size is zero");
        return false;
    }

    clear();
    m_ptr = packet;
    m_size = size;
    return true;
