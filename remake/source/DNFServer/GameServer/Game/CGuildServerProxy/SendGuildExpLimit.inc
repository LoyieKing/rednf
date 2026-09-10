void CGuildServerProxy::SendGuildExpLimit()
{
    Packet_Guild_Exp_Limit pkt;
    pkt.m_channel = G_CEnvironment()->get_channel_no();
    pkt.m_guildExpLimit8 = G_CDataManager()->m_guildParameter.m_levelUpCost[8][1];
    pkt.m_guildExpLimit16 = G_CDataManager()->m_guildParameter.m_levelUpCost[16][1];
    for (int i = 0; i < 0x11; i++)
    {
        pkt.m_guildExpLimit[i] = G_CDataManager()->m_guildParameter.m_levelUpCost[i][1];
    }
    SendTcpPacket((char*)&pkt, pkt.packetSize);
