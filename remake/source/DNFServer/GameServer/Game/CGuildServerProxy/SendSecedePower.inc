void CGuildServerProxy::SendSecedePower(unsigned int charNo, unsigned int guildKey)
{
    Packet_Request_Secede_Power pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}