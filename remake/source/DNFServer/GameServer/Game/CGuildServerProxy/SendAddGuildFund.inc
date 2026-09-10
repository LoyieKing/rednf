void CGuildServerProxy::SendAddGuildFund(unsigned int charNo, unsigned int guildKey,
                                         int fund, Packet_Add_Guild_Fund::eFundType type)
{
    Packet_Add_Guild_Fund pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_fund = fund;
    pkt.m_mode = (unsigned char)type;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
