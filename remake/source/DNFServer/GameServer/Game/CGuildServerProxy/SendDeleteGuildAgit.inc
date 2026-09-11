void CGuildServerProxy::SendDeleteGuildAgit(unsigned int guildKey, unsigned int charNo)
{
    Packet_Guild_Delete_Guild_Agit pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_charNo = charNo;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}