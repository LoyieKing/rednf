void CSchoolServerProxy::SendPacket(char* packet, int length)
{
    m_udpHandler.SendToServer(packet, length,
                               static_cast<unsigned short>(m_port),
                               m_ip.c_str());
}