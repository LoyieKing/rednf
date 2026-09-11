// throw() 与头声明同步（见 InterfacePacketBuf.h 的 ORIG EH 形证据注释）
void InterfacePacketBuf::AcquirePacketBuf(PacketBuf* packetBuf) throw()
{
    m_packetBuf = packetBuf;
}