    // [b28d-solo-AvatarConvertServer] AE（ORIG 0x08193136，49/49，p167 CLEAR）。
    // N7 间接 call 目标寄存器（ORIG 0x0819319b `mov (%eax),%edx; call *%edx`）在
    // 4.4.7 下恒 %eax；Main 已按本泳道双后端读数把本 TU 路由到 446 后端
    // （dnf_game_cxx.sh「仅换 446 后端」表），逐操作数一致。P282 已核虚槽 +0xc。
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
}