void CGuildServerProxy::SendGuildMail(unsigned int charNo, unsigned int guildId,
                                      int len, char* msg)
{
    Packet_Monitor_Send_Guild_Mail pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildId = guildId;
    memcpy(pkt.m_msg, msg, len);
    SendPacket((char*)&pkt, pkt.packetSize);
}