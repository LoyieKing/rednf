// [G7-sub 2026-09-11 判词] MonitorTcpThread::dispatch（ORIG 0x0857d95a，1575 条巨件，
// §6.2 三件套）：本轮**已改 2 类共 6 处**（1592→1577，gap +17→+2）：
//   ① 0x0857e089..0x0857e254 第二循环 4 处条件由 `&&` 改**嵌套 if**：ORIG 外条件发
//      `cmp $0xffffffff,%eax; setne %al; test %al,%al; je`、内层 FD_ISSET 直测
//      （探针 /tmp/g7c/probe4.cpp c1≡ORIG；`&&` 形 c2 恒折 `cmp; je` 且末操作数再发
//      `mov $1/jmp/mov $0/test` 四尾布尔物化岛）。
//   ② 0x0857da91..0x0857daa2 / 0x0857dd5e / 0x0857e7d3：原码为**函数级单变量 i**
//      （先独立 `i = 0;` 再 `for (i = 0; …)` ⇒ 连续两条 `movl $0,-0x58`），且两循环
//      条件内**逐次重调** `CServerProxyMgr<CMonitorServerProxy>::GetServerProxyArraySize()`
//      （该 getter 全函数仅 2 处调用，都在条件里）⇒ 删 `proxyCount` 具名缓存
//      （旧形多 1 前置 call + -0x50 槽，并把两循环索引拆成 -0x3c/-0x38 两槽）。
// 残留 2 条（均为 4.4 固有，探针实证不可翻）：
//   · 0x0857e3xx `while (ret < 0)`：4.4 发 `mov; shr $0x1f; test %al; jne` vs ORIG
//     `mov; test %eax,%eax; js`（probe5.cpp p1）。
//   · 0x0857e2xx `getsockopt(...,&soLen[3])`：4.4 发 `lea; add $0xc` vs ORIG 折单条
//     `lea`（probe5.cpp p2/p3）。
// p167：CALLSITE CLEAR（仅 C1/C2·D1/D2 同址别名，ORIG nm 实测如 0x0861c8d2）。
// 结论：语义完成，byte 层余 2 条 4.4 固有差 ⇒ 封顶。
void MonitorTcpThread::dispatch(void* arg)
{
    (void)arg;
    fd_set readfds;
    fd_set writefds;
    socklen_t soLen[4];
    struct timeval timeout;
    int soError = 0;
    // [SweepGD] ORIG 0x0857d966..0x0857d989 函数头只有 **5 条** `movl $0,-N`
    // （soError + lastReconnect×4），其余标量为**无初值裸声明**（-O0 裸声明零指令，
    // P328⑤）⇒ 去掉 socket/proxyCount/ret 的 `= 0`（我方原多 7 条清零）。
    int ret;
    int lastReconnectPcroom = 0;
    int lastReconnectCommunity = 0;
    int lastReconnectAuction = 0;
    int lastReconnectCera = 0;
    int pcroomSocket;
    int communitySocket;
    int auctionSocket;
    int ceraSocket;
    int hadesSocket;
    // [G7-sub] 原 `int proxyCount;` 已删：ORIG 循环条件内逐次重调
    // GetServerProxyArraySize（见下），无缓存局部。索引 `i` 为**函数级单变量**
    // （两循环同用 -0x58 槽：第一循环 12 次 + 第二循环 16 次引用）。
    int i;

    // [SweepGD] ORIG +0x1a32 尾条件块 `movzbl s_server_stop; xor $0x1; test %al;
    // jne <body>`（函数头 `jmp +0x1a32`）⇒ 原码是 `while (!s_server_stop)` 布尔字段
    // 裸条件（P168a①/P292），而非 `while(true){ if(stop) return; }`（后者头部多 4 个
    // jmp+nop 中继岛）。
    while (!GlobalData::s_server_stop)
    {
        do
        {
            OS_API::Sleep(1);
            pcroomSocket =
                GlobalData::s_pcroom_proxy->GetTcpHandler()->get_socket();
            communitySocket =
                GlobalData::s_community_proxy->GetTcpHandler()->get_socket();
            auctionSocket =
                GlobalData::s_auction_proxy->GetTcpHandler()->get_socket();
            ceraSocket =
                GlobalData::s_cera_auction_proxy->GetTcpHandler()->get_socket();
            hadesSocket =
                GlobalData::s_hades_proxy->GetTcpHandler()->get_socket();

            // [SweepGD 2026-09-10] ORIG 0x0857da21/0x0857da4f 的 FD_ZERO 展开 = 旧
            // glibc 逐元素循环（`lea fd; mov %eax,-0x50; movl $0,-0x54; …;
            // movl $0,(%eax,%edx,4); addl $1; cmpl $0x1f,-0x54; setbe`）——fd_set*
            // 具名局部 + unsigned 计数局部两槽；新头集宏走 `rep stos` 完全无形
            // ⇒ 手写等价形（同 UdpThread::dispatch 先例）。
            {
                unsigned int fi;
                fd_set* fds = &readfds;
                for (fi = 0; fi <= 0x1f; ++fi)
                    __FDS_BITS(fds)[fi] = 0;
            }
            {
                unsigned int fi;
                fd_set* fds = &writefds;
                for (fi = 0; fi <= 0x1f; ++fi)
                    __FDS_BITS(fds)[fi] = 0;
            }
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            int maxfd = 0;
            // [G7-sub] ORIG 0x0857da91..0x0857daa2 是**连续两条** `movl $0,-0x58(%ebp)`
            // 后才 `jmp` 条件 ⇒ 原码一个具名 `int i`（先独立 `int i = 0;`、再
            // `for (i = 0; …)`）；且循环条件内**逐次重调**
            // `call 0857f8c2 <CServerProxyMgr<CMonitorServerProxy>::GetServerProxyArraySize()>`
            // （0x0857dd66 / 第二循环 0x0857e7d3 —— 该 getter 全函数仅此 2 处，
            // 都在条件里）⇒ 禁 `proxyCount` 具名缓存（旧形多 1 次前置 call + 1 个
            // -0x50 槽，且把两循环索引拆成 -0x3c/-0x38 两槽 ⇒ 与 ORIG -0x58 单槽不符）。
            i = 0;
            for (i = 0;
                 i < GlobalData::s_monitor_proxy_mgr->GetServerProxyArraySize(); ++i)
            {
                // ORIG 0x0857daae..0x0857dc04（p167 实证：每 mgr 6 次三元链）：
                // FD_SET/FD_ISSET 的 fd 实参逐处重调
                // GetServerProxyArrayAt(i)->GetTcpHandler()->get_socket()
                // （P157 逐字段重读，禁 mon/s 缓存；glibc FD_SET 宏对 fd 双求值）。
                if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
                {
                    FD_SET((unsigned)GlobalData::s_monitor_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &readfds);
                    FD_SET((unsigned)GlobalData::s_monitor_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &writefds);
                    maxfd = std::max(
                        maxfd, GlobalData::s_monitor_proxy_mgr
                                   ->GetServerProxyArrayAt(i)
                                   ->GetTcpHandler()
                                   ->get_socket());
                }
                if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
                {
                    FD_SET((unsigned)GlobalData::s_guild_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &readfds);
                    FD_SET((unsigned)GlobalData::s_guild_proxy_mgr
                               ->GetServerProxyArrayAt(i)
                               ->GetTcpHandler()
                               ->get_socket(),
                           &writefds);
                    maxfd = std::max(
                        maxfd, GlobalData::s_guild_proxy_mgr
                                   ->GetServerProxyArrayAt(i)
                                   ->GetTcpHandler()
                                   ->get_socket());
                }
            }
            if (pcroomSocket != -1)
            {
                // [SweepGD] ORIG 0x0857dc34 段 `shr $0x5` / `and $0x1f`（无符号）⇒
                // 宏实参 (unsigned) cast（同 AcceptThread::dispatch 先例）消有符号除链。
                FD_SET((unsigned)pcroomSocket, &readfds);
                FD_SET((unsigned)pcroomSocket, &writefds);
                maxfd = std::max(maxfd, pcroomSocket);
            }
            if (communitySocket != -1)
            {
                FD_SET((unsigned)communitySocket, &readfds);
                FD_SET((unsigned)communitySocket, &writefds);
                maxfd = std::max(maxfd, communitySocket);
            }
            if (auctionSocket != -1)
            {
                FD_SET((unsigned)auctionSocket, &readfds);
                FD_SET((unsigned)auctionSocket, &writefds);
                maxfd = std::max(maxfd, auctionSocket);
            }
            if (ceraSocket != -1)
            {
                FD_SET((unsigned)ceraSocket, &readfds);
                FD_SET((unsigned)ceraSocket, &writefds);
                maxfd = std::max(maxfd, ceraSocket);
            }
            if (hadesSocket != -1)
            {
                FD_SET((unsigned)hadesSocket, &readfds);
                FD_SET((unsigned)hadesSocket, &writefds);
                maxfd = std::max(maxfd, hadesSocket);
            }
            maxfd += 1;
            ret = select(maxfd, &readfds, &writefds, 0, &timeout);
        } while (ret < 0);

        // [G7-sub] 与第一循环同一个具名 `i`（ORIG 全函数 -0x58 单槽 28 次引用），
        // 条件内重调 size（ORIG 0x0857e7d3 = 第二处也是唯一另一处调用）。
        for (i = 0;
             i < GlobalData::s_monitor_proxy_mgr->GetServerProxyArraySize(); ++i)
        {
            // ORIG 0x0857e089..0x0857e254：select 后同样逐处重调三元链
            // （if 1 次 + FD_ISSET 双求值 2 次；OnTcpRecv 前仅 1 次
            // GetServerProxyArrayAt，无 GetTcpHandler）。
            // [G7-sub] ORIG 0x0857e089 段是**嵌套 if**（非 `&&`）：外条件发
            // `cmp $0xffffffff,%eax; setne %al; test %al,%al; je`（probe
            // /tmp/g7c/probe4.cpp c1 与 ORIG 逐条同形；`&&` 形 c2 恒折成
            // `cmp; je`）⇒ 内层 FD_ISSET 直接 `and $1; test %al,%al; je`；
            // 旧 `&&` 写法另发 `mov $1/jmp/mov $0/test` 四尾布尔物化岛。
            if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
            {
                if (FD_ISSET((unsigned)GlobalData::s_monitor_proxy_mgr
                                 ->GetServerProxyArrayAt(i)
                                 ->GetTcpHandler()
                                 ->get_socket(),
                             &readfds))
                {
                    // b28c-l23 SIGFIX 消费点：OnTcpRecv 已 bool 化（lane28/lane12），
                    // [SweepGD] 实参 (unsigned) cast：ORIG `shr $0x5`/`and $0x1f` 无符号形。
                    // bool callee ⇒ 裸形（P168a①：ORIG `call; xor$1; testb; je`）。
                    if (!GlobalData::s_monitor_proxy_mgr
                             ->GetServerProxyArrayAt(i)
                             ->OnTcpRecv())
                    {
                        LogManager::logFormat(
                            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x449,
                            "Monitor::OnTcpRecv == false!!!");
                        CStreamGuard guard(
                            GlobalData::s_stream_pool->Acquire(
                                "NetworkThread.cpp", 0x44c),
                            true);
                        **guard << 0x15f;
                        **guard << -1;
                        **guard << 0xc9;
                        GlobalData::s_msgq_mgr->put(
                            MsgQueueMgr::DISPATCH_Q, guard);
                    }
                }
            }
            if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
            {
                // [G7-sub] 同 readfds 段：ORIG 0x0857e1a2 嵌套 if（外条件
                // `setne;test;je`），内层 FD_ISSET 直测。
                if (FD_ISSET((unsigned)GlobalData::s_monitor_proxy_mgr
                                 ->GetServerProxyArrayAt(i)
                                 ->GetTcpHandler()
                                 ->get_socket(),
                             &writefds))
                {
                    int sendRet = GlobalData::s_monitor_proxy_mgr
                                      ->GetServerProxyArrayAt(i)
                                      ->OnTcpSend();
                    if (sendRet < 0)
                    {
                        LogManager::logFormat(
                            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x45e,
                            "Monitor::OnTcpSend == false!!!remain_size(%d)",
                            sendRet);
                        CStreamGuard guard(
                            GlobalData::s_stream_pool->Acquire(
                                "NetworkThread.cpp", 0x460),
                            true);
                        **guard << 0x15f;
                        **guard << -1;
                        **guard << 0xc9;
                        GlobalData::s_msgq_mgr->put(
                            MsgQueueMgr::DISPATCH_Q, guard);
                    }
                }
            }
            if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
            {
                // [G7-sub] 同 monitor 段：ORIG 嵌套 if（外条件 setne;test;je）。
                if (FD_ISSET((unsigned)GlobalData::s_guild_proxy_mgr
                                 ->GetServerProxyArrayAt(i)
                                 ->GetTcpHandler()
                                 ->get_socket(),
                             &readfds))
                {
                    // b28c-l23 SIGFIX 消费点：bool callee ⇒ 裸形（P168a①）
                    if (!GlobalData::s_guild_proxy_mgr
                             ->GetServerProxyArrayAt(i)
                             ->OnTcpRecv())
                    {
                        LogManager::logFormat(
                            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x46f,
                            "Guild::OnTcpRecv == false!!!");
                        CStreamGuard guard(
                            GlobalData::s_stream_pool->Acquire(
                                "NetworkThread.cpp", 0x471),
                            true);
                        **guard << 0x15f;
                        **guard << -1;
                        **guard << 0xcb;
                        GlobalData::s_msgq_mgr->put(
                            MsgQueueMgr::DISPATCH_Q, guard);
                    }
                }
            }
            if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1)
            {
                // [G7-sub] 同 monitor 段：ORIG 嵌套 if（外条件 setne;test;je）。
                if (FD_ISSET((unsigned)GlobalData::s_guild_proxy_mgr
                                 ->GetServerProxyArrayAt(i)
                                 ->GetTcpHandler()
                                 ->get_socket(),
                             &writefds))
                {
                    int sendRet = GlobalData::s_guild_proxy_mgr
                                      ->GetServerProxyArrayAt(i)
                                      ->OnTcpSend();
                    if (sendRet < 0)
                    {
                        LogManager::logFormat(
                            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x480,
                            "Guild::OnTcpSend == false!!!remain_size(%d)",
                            sendRet);
                        CStreamGuard guard(
                            GlobalData::s_stream_pool->Acquire(
                                "NetworkThread.cpp", 0x481),
                            true);
                        **guard << 0x15f;
                        **guard << -1;
                        **guard << 0xcb;
                        GlobalData::s_msgq_mgr->put(
                            MsgQueueMgr::DISPATCH_Q, guard);
                    }
                }
            }
        }

        // HADES 代理
        if (hadesSocket != -1 &&
            GlobalData::s_hades_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET((unsigned)hadesSocket, &readfds))
        {
            if (!GlobalData::s_hades_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x493,
                    "HADES::OnTcpRecv == false!!!");
            }
        }
        if (hadesSocket != -1 && FD_ISSET((unsigned)hadesSocket, &writefds))
        {
            int sendRet = GlobalData::s_hades_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x49d,
                    "HADES::OnTcpSend == false!!!remain_size(%d)", sendRet);
            }
        }

        // PCROOM 代理
        if (pcroomSocket != -1 &&
            GlobalData::s_pcroom_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET((unsigned)pcroomSocket, &readfds))
        {
            if (!GlobalData::s_pcroom_proxy->OnTcpRecv())
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x4e0,
                    "PCROOM::OnTcpRecv == false!!!");
            }
        }
        if (pcroomSocket != -1 && FD_ISSET((unsigned)pcroomSocket, &writefds))
        {
            if (GlobalData::s_pcroom_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[3] = 4;
                ret = getsockopt(pcroomSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[3]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_pcroom_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
            }
            int sendRet = GlobalData::s_pcroom_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x4f9,
                    "PCROOM::OnTcpSend == false!!!remain_size(%d)", sendRet);
            }
        }
        // ORIG 0x0857eab0/0x0857eb12：if/else-if 两臂各自独立调用
        // GetSocketState（P48：O0 无 CSE，禁具名状态缓存）。
        if (GlobalData::s_pcroom_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectPcroom >
                0x3c)
            {
                GlobalData::s_pcroom_proxy->InitTcp();
                GlobalData::s_pcroom_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (GlobalData::s_pcroom_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectPcroom >
                0x3c)
            {
                lastReconnectPcroom =
                    GlobalData::s_systemTime_.getCurSec();
                GlobalData::s_pcroom_proxy->Disconnect();
            }
        }

        // COMMUNITY 代理（仅整合 PvP 频道时处理）
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
            if (communitySocket != -1 &&
                GlobalData::s_community_proxy->GetSocketState() ==
                    ENUM_SOCKET_STATE_CONNECTED &&
                FD_ISSET((unsigned)communitySocket, &readfds))
            {
                if (!GlobalData::s_community_proxy->OnTcpRecv())
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x521,
                        "COMMUNITY::OnTcpRecv == false!!!");
                }
            }
            if (communitySocket != -1 &&
                FD_ISSET((unsigned)communitySocket, &writefds))
            {
                if (GlobalData::s_community_proxy->GetSocketState() ==
                    ENUM_SOCKET_STATE_CONNECTING)
                {
                    soError = 0;
                    soLen[2] = 4;
                    ret = getsockopt(communitySocket, SOL_SOCKET, SO_ERROR,
                                     &soError, &soLen[2]);
                    if (soError == 0 && ret == 0)
                    {
                        GlobalData::s_community_proxy->SetSocketState(
                            ENUM_SOCKET_STATE_CONNECTED);
                    }
                }
                int sendRet =
                    GlobalData::s_community_proxy->OnTcpSend();
                if (sendRet < 0)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x537,
                        "COMMUNITY::OnTcpSend == false!!!remain_size(%d)",
                        sendRet);
                }
            }
            if (GlobalData::s_community_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_DISCONNECTED)
            {
                GlobalData::s_community_proxy->InitTcp();
                GlobalData::s_community_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectCommunity =
                    GlobalData::s_systemTime_.getCurSec();
            }
            else if (GlobalData::s_community_proxy->GetSocketState() ==
                     ENUM_SOCKET_STATE_CONNECTING)
            {
                if (GlobalData::s_systemTime_.getCurSec() -
                        lastReconnectCommunity >
                    0x3c)
                {
                    lastReconnectCommunity =
                        GlobalData::s_systemTime_.getCurSec();
                    GlobalData::s_community_proxy->Disconnect();
                }
            }
        }

        // ORIG 0x0857eda3：第二次 G_GameWorld()->IsIntegratedPvPBaseChannel()，
        // `xor $1; test %al,%al; je 循环尾` ⇒ 整合 PvP 频道时跳过
        // auction/cera 两段（与前面 community 段互补的两独立 if，P48 无 CSE）。
        if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
        // AUCTION 代理
        if (auctionSocket != -1 &&
            GlobalData::s_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET((unsigned)auctionSocket, &readfds))
        {
            if (!GlobalData::s_auction_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x734,
                    "AUCTION - OnTcpRecv == false!!!");
            }
        }
        if (auctionSocket != -1 && FD_ISSET((unsigned)auctionSocket, &writefds))
        {
            if (GlobalData::s_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[1] = 4;
                ret = getsockopt(auctionSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[1]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_auction_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
                GlobalData::s_auction_proxy->SendRegistPacket();
            }
            int sendRet = GlobalData::s_auction_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x74c,
                    "AUCTION - OnTcpSend == false!!!remain_size(%d)",
                    sendRet);
            }
        }
        if (GlobalData::s_auction_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectAuction >
                0x3c)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x754, 0);
                trace("AUCTION - invoke InitTcp() by time out or disconnect");
                GlobalData::s_auction_proxy->InitTcp(true);
                GlobalData::s_auction_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectAuction =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (GlobalData::s_auction_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectAuction >
                0x3c)
            {
                lastReconnectAuction =
                    GlobalData::s_systemTime_.getCurSec();
                cMyTrace trace(__PRETTY_FUNCTION__, 0x761, 0);
                trace("AUCTION - Disconnect to Auction Server(Timeout for connect trying)");
                GlobalData::s_auction_proxy->Disconnect();
            }
        }

        // CERA AUCTION 代理
        if (ceraSocket != -1 &&
            GlobalData::s_cera_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTED &&
            FD_ISSET((unsigned)ceraSocket, &readfds))
        {
            if (!GlobalData::s_cera_auction_proxy->OnTcpRecv())
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x76c,
                    "Cera AUCTION - OnTcpRecv == false!!!");
            }
        }
        if (ceraSocket != -1 && FD_ISSET((unsigned)ceraSocket, &writefds))
        {
            if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
                ENUM_SOCKET_STATE_CONNECTING)
            {
                soError = 0;
                soLen[0] = 4;
                ret = getsockopt(ceraSocket, SOL_SOCKET, SO_ERROR,
                                 &soError, &soLen[0]);
                if (soError == 0 && ret == 0)
                {
                    GlobalData::s_cera_auction_proxy->SetSocketState(
                        ENUM_SOCKET_STATE_CONNECTED);
                }
                GlobalData::s_cera_auction_proxy->SendRegistPacket();
            }
            int sendRet =
                GlobalData::s_cera_auction_proxy->OnTcpSend();
            if (sendRet < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x784,
                    "Cera AUCTION - OnTcpSend == false!!!remain_size(%d)",
                    sendRet);
            }
        }
        if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
            ENUM_SOCKET_STATE_DISCONNECTED)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectCera >
                0x3c)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x78c, 0);
                trace("Cera AUCTION - invoke InitTcp() by time out or disconnect");
                GlobalData::s_cera_auction_proxy->InitTcp(true);
                GlobalData::s_cera_auction_proxy->SetSocketState(
                    ENUM_SOCKET_STATE_CONNECTING);
                lastReconnectCera =
                    GlobalData::s_systemTime_.getCurSec();
            }
        }
        else if (GlobalData::s_cera_auction_proxy->GetSocketState() ==
                 ENUM_SOCKET_STATE_CONNECTING)
        {
            if (GlobalData::s_systemTime_.getCurSec() -
                    lastReconnectCera >
                0x3c)
            {
                lastReconnectCera =
                    GlobalData::s_systemTime_.getCurSec();
                cMyTrace trace(__PRETTY_FUNCTION__, 0x799, 0);
                trace("Cera AUCTION - Disconnect to Cera Auction Server(Timeout for connect trying)");
                GlobalData::s_cera_auction_proxy->Disconnect();
            }
        }
        }   // !IsIntegratedPvPBaseChannel()
    }
