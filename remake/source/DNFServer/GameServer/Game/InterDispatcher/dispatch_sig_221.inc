int Inter_VillageAttackedRewardServer::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        return 0;
    // ORIG 读 env+0x1a8 = Server_Envir(+0x198)+0x10 = m_serverEnvir.m_serverType
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
        return 0;

    // ORIG `call memset@plt`（Ghidra C `memset(local_26,0,10)`）⇒ 显式 memset 形（P253/P214③）。
    char local_26[10];
    memset(local_26, 0, 10);
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1ec);

    const int local_14 =
        GlobalData::s_villageMonsterMgr->GetRewardType();   // ORIG _ZN16village_attacked18CVillageMonsterMgr13GetRewardTypeEv
    if (local_14 == 1)
        local_26[0] = '\0';
    else if (local_14 == 2)
        local_26[0] = '\x01';
    local_26[1] = 2;
    packet.put_binary(local_26, 10);
    packet.finalize(true);

    GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
    GlobalData::s_revengeDungeonMgr->SetCloseTime(0);

    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)   // P28：ORIG 增量段 ppEi ⇒ 后置++
    {
        CUser* local_10 = it->second;
        if (local_10 != 0 && local_10->get_state() > 2)
        {
            local_10->SetCurCharacRevengeData('\0', '\0');
            local_10->VillageAttack_NotDBUpdate();
            local_10->Send(packet);
        }
    }

    GlobalData::s_villageMonsterMgr->OnEndRewardVillageMonster();
    return 0;
