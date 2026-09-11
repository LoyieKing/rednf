void CGuildServerProxy::SendPacketCallGuildAllMembers(unsigned int charNo,
                                                      unsigned int guildKey)
{
    Packet_Monitor_Call_Guild_All_Members pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendPacket((char*)&pkt, pkt.packetSize);
}