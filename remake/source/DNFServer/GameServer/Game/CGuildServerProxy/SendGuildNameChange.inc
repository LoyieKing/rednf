// ORIG @846e387：param_1 → m_charNo(+0xe)、param_2 → m_guildKey(+0xa)
// （调用点 Dispatchers.cpp:7786 传 (curNo, guildkey)）。
void CGuildServerProxy::SendGuildNameChange(unsigned int charNo, unsigned int guildKey,
                                            char* name)
{
    Packet_Change_Guild_Name pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    strcpy(pkt.m_name, name);
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}