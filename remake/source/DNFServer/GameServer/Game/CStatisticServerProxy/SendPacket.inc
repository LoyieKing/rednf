void CStatisticServerProxy::SendPacket(char* packet, int size)
{
    if (m_port != 0)
        m_udp.SendToServer(packet, size, (unsigned short)m_port, m_ip.c_str());
    return;
