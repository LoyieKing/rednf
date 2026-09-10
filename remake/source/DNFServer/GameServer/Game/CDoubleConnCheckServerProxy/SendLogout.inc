void CDoubleConnCheckServerProxy::SendLogout(unsigned int userID, int channel,
                                             int group)
{
    if (userID == 0)
    {
        return;
    }
    Packet_DoubleCheck_UDP_Logout packet;
    packet.m_connNo = userID;
    packet.m_accId = userID;
    packet.m_channel = (unsigned char)channel;
    packet.m_serverGroup = (unsigned char)group;
    m_udpHandler.SendToServer((char*)&packet, 0x10,
                              (unsigned short)m_port, m_ip.c_str());
