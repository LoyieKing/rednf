bool PacketBuf::put_header(int packetId, int packetSize)
{
    *m_ptr = (char)packetId;
    *(unsigned short*)(m_ptr + 1) = (unsigned short)packetSize;
    m_use_size = 15;
    return true;
