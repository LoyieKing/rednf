void Dispatcher_DisJointAvatar::SendDisjointAvatarInfo(CItem const* pItem)
{
    Packet_Avater_Disjoint_Statistic packet;
    packet.m_field0a = 1;
    // ORIG 0x08218996：gradeIdx 具名临时 + if/else 两段（sete 物化 + 槽往返
    // 后写回 +0x0e）；m_field0a=1 先行、m_flag=1 后行。
    int gradeIdx = 0;
    if (pItem->get_rarity() == 2)
        gradeIdx = 2;
    else
        gradeIdx = pItem->get_grade() - 1;
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = pItem->GetItemType();
    packet.m_flagA = 1;
    // 伪 helper 归一（战术四式）：ORIG 0x08218996 内联
    // GetServerProxy(s_statistic_proxy_mgr, 0) → SendPacket(buf, 0x28)，
    // 不引入统计代理伪 helper。
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x28);
