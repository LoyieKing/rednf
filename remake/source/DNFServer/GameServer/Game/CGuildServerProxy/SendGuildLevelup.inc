void CGuildServerProxy::SendGuildLevelup(int uid, unsigned int charNo,
                                         unsigned int guildKey, unsigned int fund)
{
    Packet_Monitor_Call_Guild_Level_Up pkt;
    pkt.m_uid = uid;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_fund = fund;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
