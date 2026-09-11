void CMonitorServerProxy::SendPacketCallGuildMembers(unsigned int charNo,
                                                     unsigned int guildKey)
{
    Packet_Monitor_Call_Guild_Members pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    SendPacket((char*)&pkt, pkt.packetSize);
}