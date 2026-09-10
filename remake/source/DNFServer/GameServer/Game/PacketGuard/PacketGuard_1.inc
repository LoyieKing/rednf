PacketGuard::PacketGuard(PacketBuf* packetBuf)
    : InterfacePacketBuf(), m_pool(0), m_external(true)
{
    AcquirePacketBuf(packetBuf);
