void CGuildServerProxy::SendGuildNameChange(unsigned int charNo, unsigned int guildKey,
                                            char* name)
{
    Packet_Change_Guild_Name pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    strcpy(pkt.m_name, name);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
