bool InterfacePacketBuf::bind_packet(char* packet, int size)
{
    return m_packetBuf->bind_packet(packet, size);
