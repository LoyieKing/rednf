void CDoubleConnCheckServerProxy::SendHeartBeat(int channel, int group)
{
    Packet_Double_Check_Server_UDP_HeartBeat packet;
    packet.m_connNo = 1000;
    packet.m_channel = (unsigned char)channel;
    packet.m_group = (unsigned char)group;
    m_udpHandler.SendToServer((char*)&packet, 0xc,
                              (unsigned short)m_port, m_ip.c_str());
