void CGuildServerProxy::SendApplyOriginalPowerSide(unsigned int charNo,
                                                   unsigned int guildKey,
                                                   unsigned char powerSide)
{
    Packet_Guild_Apply_Origial_Power_Side pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_powerSide = powerSide;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
