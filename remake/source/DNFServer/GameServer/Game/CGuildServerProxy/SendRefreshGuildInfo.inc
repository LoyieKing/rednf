void CGuildServerProxy::SendRefreshGuildInfo(unsigned int charNo, unsigned int guildKey)
{
    Packet_Refresh_Guild_Info pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}