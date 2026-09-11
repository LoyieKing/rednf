// ProcessServerStatusLog() —— ORIG 0x80ede91（0x4ce 字节，逐指令转写）：
// 聚合 user/party/warroom/deathtower/pvp_room/trade 池占用、消息队列/流池余量、
// IPG 状态、creature/avatar/egg 池等指标后统一 cMyTrace 输出，随后按用户数
// 低于 party/pvp/trade 池占用打印三条 SESSION ERROR 日志。
// [b28d-28] DIFF | ORIG 0x080ede91 | N82：大函数槽/视图寻址差（302 vs 296）。
// [b28d-solo-GameMain] N82 封顶 | 296/302 | 大函数帧 0x124 vs 0x12c + 槽平移；
// 池 Size()/Remain() 计算与 IPG 视图寻址展开差异（4.4 内联/槽复用策略）。
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

    // P183：ORIG 这 7 个 Size() 无具名槽（`mov %eax,%ebx` / `mov %eax,%esi` /
    // -0xa4..-0x94），系 trace 实参从右到左求值的表达式临时 ⇒ 直用实参表达式。

    {
        // [N3Free171to190·GameMain] 改临时对象形：ORIG 0x080ede91 实证 7 个 Size()
        // 实参在 ctor **之前**求值并落槽（-0xa4..-0x8c 五槽 + %ebx/%esi/%edi），
        // ctor 后才把实参搬入 0x??(%esp)；具名局部使 ctor 先执行，实参退化为纯寄存器
        // 保活（我方 299 条；ORIG 302）。临时形后 302=302（DIFF +3 → NEAR）。
        // [N3Free171to190·GameMain] P139：ORIG 发射
        // `_ZZ22ProcessServerStatusLogvE19__PRETTY_FUNCTION__`（0x08b37c70，size 0x1e
        // = "void ProcessServerStatusLog()"+NUL）——手写串内容相同但不发射该静态符号
        // （nm missing）⇒ 换回 __PRETTY_FUNCTION__（串内容与符号双对齐）。
        cMyTrace(__PRETTY_FUNCTION__, 0x5c, 0)(
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
            userUsed, G_CGameManager()->m_userPool.Size(), userCount,
            G_CGameManager()->m_userPool.Size(),
            partyUsed, G_CGameManager()->m_partyPool.Size(),
            warRoomUsed, G_CGameManager()->m_warRoomPool.Size(),
            deathTowerUsed, G_CGameManager()->m_deathTowerPool.Size(),
            pvpUsed, G_CGameManager()->m_pvpRoomPool.Size(),
            tradeUsed, G_CGameManager()->m_tradeSpacePool.Size());
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
}