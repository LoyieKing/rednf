bool MainLoop()
{
    // ORIG 0x0829fa33..0x0829fa87：函数头 12 个具名局部初始化，槽深 -0x48..-0x1c。
    // 其中 6 个为只写不读的 dead 局部（lim30/lim10/dead3/lim20/dead8/lim300）——
    // 它们撑起 ORIG 帧 0x60（我方旧版 0x50）并决定后续槽号；比较处 ORIG 仍用立即数。
    int lim30 = 30;         // -0x48（dead）
    int lim10 = 10;         // -0x44（dead）
    int cnt10 = 0;          // -0x40
    int dead3 = 0;          // -0x3c（dead）
    int lim20 = 20;         // -0x38（dead）
    int cnt30 = 0;          // -0x34
    int lastUserCount = 0;  // -0x30
    int lastRemain = 0;     // -0x2c
    int dead8 = 0;          // -0x28（dead）
    int flip24 = 0;         // -0x24
    int lim300 = 300;       // -0x20（dead）
    int cnt300 = 0;         // -0x1c
    (void)lim30; (void)lim10; (void)dead3; (void)lim20; (void)dead8; (void)lim300;

    while (!GlobalData::s_server_stop)
    {
        if (cnt30 == 30)
        {
            G_GameWorld()->UpdateChannelUser();
            int curUser = G_GameWorld()->get_UserCount_InWorld();
            int remain =
                G_CGameManager()->m_userPool.Size() - G_CGameManager()->m_userPool.Remain();
            if (lastUserCount != curUser || remain != lastRemain)
            {
                ProcessServerStatusLog();
                lastUserCount = curUser;
                lastRemain = remain;
            }
            cnt30 = 0;
        }

        // ORIG 0x0829fb02 `cmpl $0x9,-0x40; setg %al; addl $0x1,-0x40; test %al,%al;
        // je`——比较用旧值、自增在测前、真分支内重置 0 ⇒ 源形 `if (cnt10++ > 9)`。
        // 旧实现 `if(cnt10>9){…;cnt10=0;} ++cnt10;` 真分支后值为 1 ≠ ORIG 0（真语义 bug）。
        if (cnt10++ > 9)
        {
            // [b28d-solo-GameMain] 真语义 bug 修：ORIG -0x4c 是**独立计数器**
            // （GetNextIndex(int&) 的引用出参，初值 0，cond 用 `m_count > next`）；
            // 旧实现 `mi = GetNextIndex(mi)` 把 proxy 索引当计数器自增（首轮即
            // 跳过 indexArray[1]，且 cond 用索引值比较）⇒ 漏发心跳。
            // 链式单表达式（P52）：get_channel_no 实参先物化 %ebx 跨 GetServerProxy
            // 保活（旧形经具名 mp 局部槽化 + 调用序反）。
            int next = 0;
            for (int mi = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
                 GlobalData::s_monitor_proxy_mgr->GetEndIndex() > next; )
            {
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)mi)->SendHeartBeat(
                        G_CEnvironment()->get_channel_no());
                mi = GlobalData::s_monitor_proxy_mgr->GetNextIndex(next);
            }
            // ORIG 两循环复用同一 -0x4c 槽（guild 前重新置 0）⇒ 用同一局部复位。
            next = 0;
            for (int gi = GlobalData::s_guild_proxy_mgr->GetStartIndex();
                 GlobalData::s_guild_proxy_mgr->GetEndIndex() > next; )
            {
                GlobalData::s_guild_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)gi)->SendHeartBeat(
                        G_CEnvironment()->get_channel_no());
                gi = GlobalData::s_guild_proxy_mgr->GetNextIndex(next);
            }
            cnt10 = 0;
        }

        if (flip24 == 1)
        {
            GlobalData::s_channel_proxy->SendChannelInfo();
            flip24 = 0;
        }
        else
        {
            flip24 = 1;
        }

        if (cnt300 == 300)
        {
            // [b28d-solo-GameMain] P259：ORIG `call Acquire` 返回值经 %eax 直传
            // CStreamGuard ctor 实参（无 s 具名局部槽往返）；旧形多 store+load 2 条。
            CStreamGuard guard(
                GlobalData::s_small_stream_pool->Acquire("App.cpp", 0x2728), true);
            **guard << 0x30a;
            **guard << -1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            cnt300 = 0;
        }
        ++cnt300;

        sleep_ext(1, 0);
        ++cnt30;
        ProcessAntiBotInputWrapper();
    }
    return 1;
