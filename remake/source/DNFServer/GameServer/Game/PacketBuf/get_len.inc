int PacketBuf::get_len() const
{
    if (m_finalized == false)
    {
        LogManager::logFormat(1, "packet_buf.cpp",
                              __PRETTY_FUNCTION__, 0x338,
                              "this packet is not finalized");
        return 0;
    }
    return m_size;
