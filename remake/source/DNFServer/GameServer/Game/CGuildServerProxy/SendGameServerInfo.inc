void CGuildServerProxy::SendGameServerInfo(char* name, unsigned short port,
                                           unsigned char group, unsigned char channel)
{
    Packet_Game_Server_Regist pkt;
    pkt.m_group = group;
    pkt.m_channel = channel;
    pkt.m_type = 1;
    pkt.m_port = port;
    strncpy(pkt.m_name, name, 0x10);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
