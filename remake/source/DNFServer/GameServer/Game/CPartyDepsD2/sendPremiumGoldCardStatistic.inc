void CParty::sendPremiumGoldCardStatistic(int type, int value1, int value2)
{
    Packet_Goldcard_Event_Statistic_GTS packet;
    packet.m_stat.m_field0 = (unsigned char)type;
    packet.m_stat.m_field1 = value1;
    packet.m_stat.m_field5 = value2;
    // [b28d-51] P171 照抄：ORIG 无 proxy 判空（GetServerProxy 结果直接作 receiver，
    // 单表达式链 + &packet 留 %ebx 跨 call）；具名 proxy 局部引入 -0xc 槽、帧 +4。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket((char*)&packet, 0x13);
}