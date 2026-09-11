// ---------------------------------------------------------------------------
// dispatch —— ORIG T 0x0857d5be：网络收包线程。消费 AcceptThread 入队的
// s_accept_que（加锁逐个 SetEpoll），随后 epoll_wait(600, 100ms) 处理
// EPOLLIN(1)/EPOLLOUT(4)/错误掩码(0x18)；sendSize>0x6ddd0(450000) 判定
// 发送缓冲过大 → DisConnSig(6)。
// ---------------------------------------------------------------------------
// [b28d-solo-GlobalData] 修结构差（p167 29/29 CLEAR）：①s_server_stop 检查搬到
// 循环尾（ORIG 0x0857d89d `movzbl s_server_stop; xor $1; test; jne body` 为回边
// 目标，trace 落循环外函数尾 0x0857d8af）⇒ `while(!stop){body} trace;`；
// ②去 epoll 具名局部（P183，ORIG 0x0857d5c6 直 `call G_EpollHandler; mov %eax,(%esp)`）。
// [SweepCAutoPunish 2026-09-10 已修·AE 207/208] 旧判「gap=6/N82 槽位封顶」作废：
// ORIG 循环前 `movl $0,-0x1c`/`movl $0,-0x18` 与内层 `movl $0,-0x14` 是 P0 具名局部
// 直证 ⇒ 补函数级 user/nfds + 内层 quser + err 预清两句式（P198①）；err 守卫改
// P225 while 物化（ORIG 0x0857d68c setne/test/je）；sendSize 分支改正极性
// `if (>){DisConn}else{send}` 复现 ORIG 0x0857d80b `jbe` 落点（P47/P220）。
// compare_tu 已判 AE（.o 重定位标签伪差）；余 1 条 = N7 寄存器角色。
// [G7-sub 2026-09-11 判词] NetworkThread::dispatch（ORIG T 0x0857d5be）：私有 .o 实测
// ORIG 208 / 我方 208（gap 0）。助记符差 2 处，均为 `nop`：我方函数中段 1 条对齐 nop、
// ORIG 尾 1 条（§6.1 纯对齐填充豁免）；无缺失/多余指令、无调用点差
// （p167 仅 cMyTrace C1↔C2 同址别名 0x0854f746）。结论：**封顶，无改动**。
void NetworkThread::dispatch(void* arg)
{
    (void)arg;
    // [b28d-solo-GlobalData] P183：ORIG 0x0857d5c6 无 epoll 具名局部
    // （`call G_EpollHandler; mov %eax,(%esp); call GetEpollEvents`）。
    epoll_event* events = G_EpollHandler()->GetEpollEvents();
    int epollfd = G_EpollHandler()->GetEpollFD();
    // ORIG 0x0857d5e6/0x0857d5ed：循环前 `movl $0,-0x1c` / `movl $0,-0x18`
    // 两处槽清零 ⇒ 原码在外层 for 用的 CUser* 与 nfds 均为**函数级具名局部**
    // 并在声明处初始化（P0 槽往返直证）。当前缺这两条 store ⇒ 槽图整体上移。
    CUser* user = 0;
    int nfds = 0;

    // [b28d-solo-GlobalData] ORIG 0x0857d89d：s_server_stop 检查在**循环尾**
    // （`movzbl s_server_stop; xor $1,%eax; test %al,%al; jne body`，+0x2df 即
    // 回边目标），trace 落循环外函数尾 ⇒ 原码 `while (!stop) { body } trace;`。
    // 循环头 `if(stop){trace;return;}` 会把检查搬到头部并多出 movzbl/test 段。
    while (!GlobalData::s_server_stop)
    {
        *__errno_location() = 0;
        OS_API::Sleep(10);

        if (!GlobalData::s_accept_que.empty())
        {
            GlobalData::s_accept_lock.lock();
            // ORIG 0x0857d633：内层 front 结果槽 -0x14 在 lock 后先清零
            // （P0 具名局部，声明在 while 之前）。
            CUser* quser = 0;
            while (!GlobalData::s_accept_que.empty())
            {
                quser = GlobalData::s_accept_que.front();
                // ORIG 0x0857d650：`movl $0,-0x10` 先于 SetEpoll 调用
                // ⇒ 预清必写两句式（P198①），非初始化式。
                int err = 0;
                err = G_EpollHandler()->SetEpoll(
                    quser, quser->m_network.get_socket(), false);
                // ORIG 0x0857d68c：`cmpl $0,-0x10; setne %al; test %al,%al;
                // je` ⇒ P225 物化形（`while(v!=0){body;break;}`；if 形恒折直跳）。
                while (err != 0)
                {
                    LogManager::logFormat(
                        1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x2c4,
                        "(m_id: %s) G_EpollHandler()->SetEpoll(pUser->get_socket(%d)) %d(%s)",
                        NumberToString(quser->get_acc_id(), 0),
                        quser->m_network.get_socket(), err, strerror(err));
                    break;
                }
                GlobalData::s_accept_que.pop();
            }
            GlobalData::s_accept_lock.unlock();
        }

        nfds = epoll_wait(epollfd, events, 600, 100);
        if (nfds == 0)
        {
            continue;
        }
        if (nfds < 0 && errno != EINTR && errno != 0)
        {
            return;
        }
        for (int i = 0; i < nfds; ++i)
        {
            user = (CUser*)events[i].data.ptr;
            if (user == 0)
            {
                continue;
            }
            if (events[i].events & 1)
            {
                user->Recv();
            }
            if (user->m_network.m_sendSize != 0 && (events[i].events & 4))
            {
                // ORIG 0x0857d80b：`cmp $0x6ddd0,%eax; jbe 0857d837`（<= 跳
                // send_packet，DisConnSig 块 fall-through 在前）⇒ 源形是
                // 正极性 `if (>){DisConn}else{send}`（P47/P220 布局翻转）。
                if (user->m_network.m_sendSize > 0x6ddd0)
                {
                    user->DisConnSig((DISCONN_SIG)6, false, 0);
                }
                else
                {
                    user->m_network.send_packet();
                }
            }
            if (events[i].events & 0x18)
            {
                user->DisConnSig((DISCONN_SIG)9, false, 0);
            }
        }
    }
    // ORIG 0x0857d8af：循环退出后统一 trace + 落函数尾（P223 尾岛 nop）。
    cMyTrace trace(__PRETTY_FUNCTION__, 0x2fe, 0);
    trace("RecvThread Terminate");
}