void CHadesServerProxy::SendHadesUdpPacket(char* packet)
{
    std::string serverIP = GetServerIP();
    m_udpHandler.SendToServer(packet, ((PacketHeader*)packet)->packetSize,
                              (unsigned short)m_udpPort, serverIP.c_str());
