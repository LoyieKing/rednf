int Inter_VillageAttackedStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;

    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||                 // env+0x1a8（NEEDS_DECL 字段化）
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;

    if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
    {
        G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, false);
        G_GameWorld()->send_all_dungeon_inout_message(0x2aff, false);
        // [b28d-lane2] P221：ORIG 0x084df548 trace 占 -0x2c、else 支 -0x1c、packet -0x38
        // （深→浅）。具名 cMyTrace 使 4.4 槽向翻转（trace -0x38/packet -0x28）；
        // 改全表达式临时链复现 ORIG 槽序。
        cMyTrace(__PRETTY_FUNCTION__, 0x4b9a, 0)(
            "ULTIMATE_LOG : VILLAGE ATTACKTED START!! ULTIMATE DUNGEON CLOSE!!");
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x4b9d, 0)(
            "ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON CLOSE!!");
    }

    // ORIG 0x084df51f：pPkt 槽 -0xc 在 trace 区后才物化（技巧13）。
    Inter_VillageAttackedStartPktView* pPkt = reinterpret_cast<Inter_VillageAttackedStartPktView*>(data);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xf1);
    packet.put_int(0);
    packet.put_int(pPkt->m_a);
    packet.put_int(pPkt->m_e);
    packet.put_int(pPkt->m_12);
    packet.put_int(0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    GlobalData::s_villageMonsterMgr->OnStartVillageAttacked();
    return 0;
