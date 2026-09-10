// [G7-sub 2026-09-11 判词] AcceptThread::dispatch（ORIG T 0x0857cc5a）：本轮**已改**。
// 修 ①（0x0857d526..0x0857d538，ORIG 直证）：原码是**循环条件形**
// `while (!G_CEnvironment()->get_suspend_state())`（cond 真时直落 0x0857d53e 退出段），
// 旧写 `while(true){ if(get_suspend_state()) continue; }`（P222 反形）多 3 尾 + 两处
// `jmp+nop` 中继岛。实测 522→515 条（ORIG 519，gap −3→+4）。
// 残留（2 处，均为分支方向/布局，语义等价，N 类）：
//   · 0x0857d533 段 ORIG 第二条件为 `and $1; xor $1; jne <loop top>`（否定包裹），
//     我方 `and $1; test; je`（probe：4.4 对 `&&` 末操作数恒发 je，非 xor 形）。
//   · 0x0857d4xx 块序（我方 jmp/mov 前置）⇒ 布局差。
// p167 调用点差集仅 PacketGuard D1↔D2 同址别名（ORIG nm 实测 0x0858de80）。
// 结论：语义完成；byte 层余 4 条 N 封顶。
void AcceptThread::dispatch(void* arg)
{
    (void)arg;
    int listenSocket = -1;
    int sockOpt = 60000;
    unsigned short port = (unsigned short)G_CEnvironment()->get_tcp_port();
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0)
    {
        LogManager::logFormat(
            1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x18e,
            "[AcceptThread::dispatch] Fail to Create ListenSocket.");
    }
    else
    {
        // ORIG 0857cc5a：栈槽 -0x74 sockaddr_in；写序 family(movw $2) → s_addr(htonl,
        // -0x70) → sin_port(htons, -0x72)，见 AcceptThread/dispatch.md +0x09b..+0x0ad。
        struct sockaddr_in addr;
        std::memset(&addr, 0, 0x10);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(0);
        addr.sin_port = htons(port);
        int setRet = setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR,
                                &sockOpt, 4);
        if (setRet < 0)
        {
            LogManager::logFormat(
                1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x19b,
                "[Error] listen socket setsockopt error");
        }
        else
        {
            if (bind(listenSocket, reinterpret_cast<struct sockaddr*>(&addr), 0x10) < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1a1,
                    "Fail to bind Listen - %s", strerror(*__errno_location()));
            }
            else if (listen(listenSocket, 0x400) < 0)
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1a7,
                    "Fail to Create AcceptThread.");
            }
            else if (!CNetwork<4096, 450000>::SetNonBlock(listenSocket))
            {
                LogManager::logFormat(
                    1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1ae,
                    "Fail to SetNonBlock AcceptThread.");
            }
            else
            {
                // ---- select/accept 循环 ----
                // [G7-sub] ORIG 0x0857d526..0x0857d538 是**循环条件形**：`call
                // G_CEnvironment; call get_suspend_state; xor $0x1,%eax; test %al,%al;
                // jne 0857ce75 <body>`（体尾 0x0857d520/0x0857d523 `jmp 0857d526`
                // 回边；cond 为真时直落 0x0857d53e 退出段）⇒ 原码
                // `while (!G_CEnvironment()->get_suspend_state())`。
                // 旧写 `while (true) { if (get_suspend_state()) continue; … }`（P222 反形）
                // 多 `test/je/jmp` 三尾 + 两处 `jmp+nop` 中继岛（我方 +11 条）。
                while (!G_CEnvironment()->get_suspend_state())
                {
                    socklen_t addrLen = 0x10;
                    fd_set readfds;
                    // ORIG 0857ce75..+0x283：FD_ZERO 展开=旧 glibc 宏循环
                    // （fd_set* 局部 + unsigned i 局部，逐 __fd_mask 清零）；
                    // 新 glibc 宏展开为 rep stos，须手写旧宏形。
                    {
                        unsigned int i;
                        fd_set* fds = &readfds;
                        for (i = 0; i < sizeof(fd_set) / sizeof(__fd_mask); ++i)
                            __FDS_BITS(fds)[i] = 0;
                    }
                    FD_SET((unsigned)listenSocket, &readfds);
                    struct timeval timeout;
                    timeout.tv_sec = 1;
                    timeout.tv_usec = 0;
                    int selRet = select(listenSocket + 1, &readfds, 0, 0,
                                        &timeout);
                    if (selRet >= 0 &&
                        FD_ISSET((unsigned)listenSocket, &readfds))
                    {
                        struct sockaddr_in peerAddr;
                        int conn = accept(listenSocket,
                                          (struct sockaddr*)&peerAddr,
                                          &addrLen);
                        if (conn < 0)
                        {
                            LogManager::logFormat(
                                1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x1d4,
                                "Accept Socket[%d] Error(%s)", conn,
                                strerror(*__errno_location()));
                        }
                        else
                        {
                            // ORIG +0x3??：inet_ntoa 结果留在 %ebx 跨 G_CEnvironment
                            // 调用保活（无栈槽往返）⇒ 内联回 strcmp 实参位（P0/P52）。
                            // ORIG +0x38b：sete 物化 + 相等路径 fall-through
                            // close+循环尾（P59 fail 前置），故条件极性 ==0。
                            if (strcmp(
                                    G_CEnvironment()->get_system_monitoring_ip(),
                                    inet_ntoa(peerAddr.sin_addr)) == 0)
                            {
                                close(conn);
                                continue;
                            }
                            {
                                if (G_CEnvironment()->m_serverEnvir.m_acceptLockDBQueueCount != 0)
                                {
                                    // ORIG +0x3bc：槽预清 0 后再存（两句式，P197③）
                                    int remain = 0;
                                    remain = (int)GlobalData::s_msgq_mgr->remain(
                                        MsgQueueMgr::DB_Q);
                                    if (m_acceptFlag == 0 &&
                                        remain >=
                                            G_CEnvironment()->m_serverEnvir.m_acceptLockDBQueueCount)
                                    {
                                        m_acceptFlag = true;
                                    }
                                    else if (m_acceptFlag &&
                                             remain <
                                                 G_CEnvironment()->m_serverEnvir.m_acceptUnlockDBQueueCount)
                                    {
                                        m_acceptFlag = false;
                                    }
                                }
                                int used = G_CGameManager()->m_userPool.Size() -
                                           G_CGameManager()->m_userPool.Remain();
                                if (m_acceptFlag || used >=
                                    G_CEnvironment()->m_serverEnvir.m_maxUserNum)
                                {
                                    // ORIG +0x4??：strcmp 结果 setne 物化 + 第二条件
                                    // setle 物化（每条件各 test/setcc/je）⇒ 源形为
                                    // 嵌套 if（P225 推广：`&&` 合并被 4.4 折成直跳），
                                    // inet_ntoa 同样内联无具名局部。
                                    if (strcmp("192.168.30.146",
                                               inet_ntoa(peerAddr.sin_addr)) != 0)
                                    {
                                        if (G_CEnvironment()->m_serverEnvir.m_maxUserNum + 5 <= used)
                                        {
                                        PacketGuard guard;
                                        guard.put_header(1, 1);
                                        guard.put_byte(0);
                                        guard.put_byte(4);
                                        guard.finalize(true);
                                        write(conn, guard.get_packet(0),
                                              guard.get_len());
                                        close(conn);
                                        cMyTrace trace(__PRETTY_FUNCTION__, 0x211, 0);
                                        trace("[ACCEPT] LOCK STATE(Curr Session : %d)",
                                              used);
                                        continue;
                                        }
                                    }
                                    if (GameserverDemonManagerInstance()
                                            ->IsProhibitConnectState())
                                    {
                                        PacketBuf pkt;
                                        pkt.put_header(1, 1);
                                        pkt.put_byte(0);
                                        pkt.put_byte(4);
                                        pkt.finalize(true);
                                        write(conn, pkt.get_packet(0), pkt.get_len());
                                        close(conn);
                                        cMyTrace trace(__PRETTY_FUNCTION__, 0x230, 0);
                                        trace("[ACCEPT DENY] ProhibitUserConnect Signal STATE(Curr Session : %d)",
                                              used);
                                    }
                                    else
                                    {
                                        CUser* user = G_CGameManager()->createUser();
                                        if (user == 0)
                                        {
                                            PacketGuard guard;
                                            guard.put_header(1, 1);
                                            guard.put_byte(0);
                                            guard.put_byte(4);
                                            guard.finalize(true);
                                            write(conn, guard.get_packet(0),
                                                  guard.get_len());
                                            close(conn);
                                            cMyTrace trace(__PRETTY_FUNCTION__, 0x277, 0);
                                            trace("[ACCEPT] fail create user(Curr Session : %d)",
                                                  used);
                                        }
                                        else
                                        {
                                            user->InitOnAccept(conn);
                                            user->set_public_ip_address(
                                                (int)peerAddr.sin_addr.s_addr);
                                            GlobalData::s_accept_lock.lock();
                                            GlobalData::s_accept_que.push(user);
                                            GlobalData::s_accept_lock.unlock();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        close(listenSocket);
    }
