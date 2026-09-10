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
    int proxyCount;

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
            proxyCount =
                GlobalData::s_monitor_proxy_mgr->GetServerProxyArraySize();
            for (int i = 0; i < proxyCount; ++i)
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

        proxyCount =
            GlobalData::s_monitor_proxy_mgr->GetServerProxyArraySize();
        for (int i = 0; i < proxyCount; ++i)
        {
            // ORIG 0x0857e089..0x0857e254：select 后同样逐处重调三元链
            // （if 1 次 + FD_ISSET 双求值 2 次；OnTcpRecv 前仅 1 次
            // GetServerProxyArrayAt，无 GetTcpHandler）。
            if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1 &&
                FD_ISSET((unsigned)GlobalData::s_monitor_proxy_mgr
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
            if (GlobalData::s_monitor_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1 &&
                FD_ISSET((unsigned)GlobalData::s_monitor_proxy_mgr
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
            if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1 &&
                FD_ISSET((unsigned)GlobalData::s_guild_proxy_mgr
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
            if (GlobalData::s_guild_proxy_mgr->GetServerProxyArrayAt(i)
                        ->GetTcpHandler()
                        ->get_socket() != -1 &&
                FD_ISSET((unsigned)GlobalData::s_guild_proxy_mgr
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
