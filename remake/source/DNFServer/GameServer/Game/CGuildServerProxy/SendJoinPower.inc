void CGuildServerProxy::SendJoinPower(unsigned int charNo, unsigned int guildKey,
                                      unsigned char powerSide)
{
    Packet_Request_Join_Power pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_powerSide = powerSide;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}