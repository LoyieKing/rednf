void CGuildServerProxy::SendPowerWarCfg(unsigned short endKillPoint, int basic,
                                        int first, int decrease, int ranking)
{
    Packet_Notice_Power_War_CFG pkt;
    pkt.m_endKillPoint = endKillPoint;
    pkt.m_basic = basic;
    pkt.m_first = first;
    pkt.m_decrease = decrease;
    pkt.m_ranking = ranking;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
