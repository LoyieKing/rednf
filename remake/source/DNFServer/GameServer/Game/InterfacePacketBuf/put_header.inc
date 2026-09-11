bool InterfacePacketBuf::put_header(int packetId, int packetSize)
{
    return m_packetBuf->put_header(packetId, packetSize);
}