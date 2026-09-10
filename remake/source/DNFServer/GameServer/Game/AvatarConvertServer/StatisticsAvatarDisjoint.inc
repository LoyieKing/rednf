void AvatarConvertServer::StatisticsAvatarDisjoint(const CItem* item,
                                                   int isRechargeStone)
{
    // ORIG 0x08193136：Packet_Avater_Disjoint_Statistic（W C1 0x08110bae）构造
    // 于 -0x34；m_field0a=1 先行；gradeIdx 具名临时 + if/else 两段（sete 物化 +
    // 槽往返写回 +0x0e）；m_pad17 ← isRechargeStone（4 字节写 +0x17）；代理内联。
    Packet_Avater_Disjoint_Statistic packet;
    packet.m_field0a = 1;
    int gradeIdx = 0;
    if (item->get_rarity() == 2)
        gradeIdx = 2;
    else
        gradeIdx = item->get_grade() - 1;
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = item->GetItemType();
    packet.m_flagA = 1;
    packet.m_pad17 = static_cast<unsigned int>(isRechargeStone);
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x28);
