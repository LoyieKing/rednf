void CGuildServerProxy::SendIncreaseGuildExp(unsigned int charNo, unsigned int guildKey,
                                             unsigned int addExp, char notify)
{
    Packet_Monitor_Increase_Guild_Exp pkt;
    // ORIG @846ece2：param_1 → m_guildKey(+0xe)，param_2 → m_charNo(+0xa)
    pkt.m_charNo = guildKey;
    pkt.m_guildKey = charNo;
    pkt.m_addExp = addExp;
    pkt.m_expType = 1;
    pkt.m_notify = notify;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}