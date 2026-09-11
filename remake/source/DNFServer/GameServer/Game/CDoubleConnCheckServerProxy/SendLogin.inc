void CDoubleConnCheckServerProxy::SendLogin(unsigned int userID, int channel,
                                            int group)
{
    Packet_DoubleCheck_UDP_Login packet;
    packet.m_connNo = userID;
    packet.m_userID = userID;
    packet.m_channel = (unsigned char)channel;
    packet.m_group = (unsigned char)group;
    m_udpHandler.SendToServer((char*)&packet, 0x10,
                              (unsigned short)m_port, m_ip.c_str());
}