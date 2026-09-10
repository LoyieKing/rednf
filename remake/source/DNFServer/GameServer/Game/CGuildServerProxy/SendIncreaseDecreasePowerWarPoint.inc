void CGuildServerProxy::SendIncreaseDecreasePowerWarPoint(unsigned int guildKey,
                                                          unsigned int charNo,
                                                          int point)
{
    Packet_Guild_Increase_Decrease_Power_War_Point pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_charNo = charNo;
    pkt.m_point = point;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
