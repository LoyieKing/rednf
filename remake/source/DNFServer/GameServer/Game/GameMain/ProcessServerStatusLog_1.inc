void ProcessServerStatusLog()
{
    int userCount = G_GameWorld()->get_UserCount_InWorld();          // -0x6c
    int userUsed = G_CGameManager()->m_userPool.Size() -
                   G_CGameManager()->m_userPool.Remain();            // -0x68
    int partyUsed = G_CGameManager()->m_partyPool.Size() -
                    G_CGameManager()->m_partyPool.Remain();          // -0x64
    int warRoomUsed = G_CGameManager()->m_warRoomPool.Size() -
                      G_CGameManager()->m_warRoomPool.Remain();      // -0x60
    int deathTowerUsed = G_CGameManager()->m_deathTowerPool.Size() -
                         G_CGameManager()->m_deathTowerPool.Remain();// -0x5c
    int pvpUsed = G_CGameManager()->m_pvpRoomPool.Size() -
                  G_CGameManager()->m_pvpRoomPool.Remain();          // -0x58
    int tradeUsed = G_CGameManager()->m_tradeSpacePool.Size() -
                    G_CGameManager()->m_tradeSpacePool.Remain();     // -0x54
    int msgq1 = static_cast<int>(GlobalData::s_msgq_mgr->remain(
        MsgQueueMgr::QUEUE_IDX::DISPATCH_Q));                      // -0x50
    int msgq2 = static_cast<int>(GlobalData::s_msgq_mgr->remain(
        MsgQueueMgr::QUEUE_IDX::DB_Q));                              // -0x4c
    int streamRemain = GlobalData::s_stream_pool->Remain();          // -0x48
    int streamSize = GlobalData::s_stream_pool->Size();              // -0x44
    int smallRemain = GlobalData::s_small_stream_pool->Remain();     // -0x40
    int smallSize = GlobalData::s_small_stream_pool->Size();         // -0x3c
    int bigRemain = GlobalData::s_big_stream_pool->Remain();         // -0x38
    int bigSize = GlobalData::s_big_stream_pool->Size();             // -0x34

    WongWork::IPG::stIPGStatus_t status;                             // -0x88
    GlobalData::s_pIPGHelper->getIPGStatus(status);

    int avatarRemain = WongWork::CAvatarItemMgr::m_AvatarItemPool.Remain();    // -0x30
    int avatarSize = WongWork::CAvatarItemMgr::m_AvatarItemPool.Size();        // -0x2c
    int creatureRemain = user_creature::GetCreaturePoolRemainSize(); // -0x28
    int creatureSize = user_creature::GetCreaturePoolSize();         // -0x24
    int eggRemain = user_creature::GetCreatureEggPoolRemainSize();   // -0x20
    int eggSize = user_creature::GetCreatureEggPoolSize();           // -0x1c

    int tradeSize = G_CGameManager()->m_tradeSpacePool.Size();       // ebx
    int pvpSize = G_CGameManager()->m_pvpRoomPool.Size();            // esi
    int deathTowerSize = G_CGameManager()->m_deathTowerPool.Size();  // -0xa4
    int warRoomSize = G_CGameManager()->m_warRoomPool.Size();        // -0xa0
    int partySize = G_CGameManager()->m_partyPool.Size();            // -0x9c
    int userSize = G_CGameManager()->m_userPool.Size();              // -0x98
    int userSize2 = G_CGameManager()->m_userPool.Size();             // -0x94

    {
        cMyTrace trace("void ProcessServerStatusLog()", 0x5c, 0);    // -0x7c
        trace(
            "\n[DISPATCHER_Q (%3d) DB_Q (%3d) IPGQueue_Size (%3d)]\n"
            "[Small_Pool (%3d/%3d) Default_Pool (%3d/%3d) Big_Pool (%3d/%3d) "
            "IPGPool_Size(%3d/%3d)]\n"
            "[AvatarPool_Size(%3d/%3d), CreaturePoolSize(%3d/%3d), "
            "CreatureEggPoolSize(%3d/%3d)]\n"
            "[Sessions (%3d/%3d) Users (%3d/%3d) Parties (%3d/%3d) "
            "WarRooms (%3d/%3d) DeathTowers (%3d/%3d) PVPs (%3d/%3d) "
            "Trades (%3d/%3d)]",
            msgq1, msgq2, status.m_field08,
            smallSize, smallRemain, streamSize, streamRemain,
            bigSize, bigRemain, status.m_field04, status.m_field00,
            avatarSize, avatarRemain, creatureSize, creatureRemain,
            eggSize, eggRemain,
            userUsed, userSize2, userCount, userSize,
            partyUsed, partySize, warRoomUsed, warRoomSize,
            deathTowerUsed, deathTowerSize, pvpUsed, pvpSize,
            tradeUsed, tradeSize);
    }

    if (userCount < partyUsed)
    {
        LogManager::logFormat(1, "localchina/localchina_App.cpp",
                              "void ProcessServerStatusLog()", 0x77,
                              "[SESSION ERROR] CurrentUserCount(%d) < CurrentPartyCount(%d)",
                              userCount, partyUsed);
    }
    if (userCount < pvpUsed)
    {
        LogManager::logFormat(1, "localchina/localchina_App.cpp",
                              "void ProcessServerStatusLog()", 0x79,
                              "[SESSION ERROR] CurrentUserCount(%d) < CurrentPVPCount(%d)",
                              userCount, pvpUsed);
    }
    if (userCount < tradeUsed)
    {
        LogManager::logFormat(1, "localchina/localchina_App.cpp",
                              "void ProcessServerStatusLog()", 0x7b,
                              "[SESSION ERROR] CurrentUserCount(%d) < CurrentTraceSpaceCount(%d)",
                              userCount, tradeUsed);
    }
