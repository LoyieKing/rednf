void CGuildServerProxy::SendBuyGuildSkill(unsigned int charNo, unsigned int guildKey,
                                          int skillId, int skillLevel, short slot,
                                          unsigned int cost)
{
    Packet_Monitor_Buy_Guild_Skill pkt;
    pkt.m_charNo = charNo;
    pkt.m_guildKey = guildKey;
    pkt.m_skillId = skillId;
    pkt.m_skillLevel = skillLevel;
    pkt.m_slot = slot;
    pkt.m_cost = cost;
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}