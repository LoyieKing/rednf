void CDoubleConnCheckServerProxy::SendPacket(char* packet, int size)
{
    m_udpHandler.SendToServer(packet, size,
                              (unsigned short)m_port, m_ip.c_str());
}