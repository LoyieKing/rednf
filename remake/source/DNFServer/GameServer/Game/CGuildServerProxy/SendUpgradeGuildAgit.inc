void CGuildServerProxy::SendUpgradeGuildAgit(unsigned int guildKey, unsigned int charNo,
                                             unsigned int powerWarPoint, unsigned int fund)
{
    Packet_Guild_Upgrade_Guild_Agit pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_charNo = charNo;
    pkt.m_powerWarPoint = powerWarPoint;
    pkt.m_fund = fund;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
