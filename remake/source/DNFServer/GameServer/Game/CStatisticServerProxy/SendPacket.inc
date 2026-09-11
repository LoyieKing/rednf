// ORIG _ZN21CStatisticServerProxy10SendPacketEPci（0x08472044，T）：
// m_port 非 0 时经 CUdpHandler 发 UDP 包（端口取 m_port 低 16 位，IP 为 m_ip）。
void CStatisticServerProxy::SendPacket(char* packet, int size)
{
    if (m_port != 0)
        m_udp.SendToServer(packet, size, (unsigned short)m_port, m_ip.c_str());
    return;
}