// ---------------------------------------------------------------------------
// dispatch —— ORIG 0x0857c9de：bind UDP 口后 select 轮询 recv，转入
// _processUDPData；循环条件 !GlobalData::s_server_stop（0x940bd64）。
// ---------------------------------------------------------------------------
// [b28d-18 N82] p167 CLEAR（8/8）；残差=帧槽（N82） | ORIG 0x0857c9de
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR；残差=N82 槽位/寄存器分配。
// [L8 2026-09-10] ORIG 0x0857ca44 段实证：4.1 的 FD_ZERO 展开为**逐元素循环**
//   （`lea -0xc8,%eax; mov %eax,-0x24; movl $0x0,-0x28; …; movl $0x0,(%eax,%edx,4);
//   addl $1,-0x28; cmpl $0x1f,-0x28; setbe`）——即「具名 fd_set* 局部 + 独立循环
//   计数」两槽（-0x24/-0x28）；我方新头集 FD_ZERO 走 rep stos（0x22 段
//   `mov $0x20,%ecx; cld; rep stos`）完全无形且少这两槽，并把 readfds/buf 顶浅 0xc。
//   同时 ORIG 两处 for 使用**各自独立**的循环变量（-0x20/-0x1c 各 6 次），我方合并
//   为单个 `i`（-0x2c×12）⇒ 改为块内独立声明。
// [G7-sub 2026-09-11 判词] UdpThread::dispatch（ORIG T 0x0857c9de）：私有 .o 实测
// ORIG 138 / 我方 136（gap 2，我方短）。逐条对照残差 = 三类：
//   ① ORIG 0x0857c9ea `movl $0x1,-0x38(%ebp)` 与 0x0857ca1a `movl $0x0,-0x30(%ebp)`
//      是两处**只写不读**的具名 dead 标量（全函数无第二处引用）；是否补 dead 局部
//      由父代理裁决 ⇒ 本泳道**不改**（补则帧内 tv_sec/tv_usec/maxfd 槽由
//      -0x3c/-0x38/-0x34 下移到 ORIG 的 -0x44/-0x40/-0x48）。
//   ② FD_ZERO 内层 ORIG `movl $0,(%eax,%edx,4)` scaled 直存 vs 我方 `shl $2; add` ⇒
//      已知 N125/N81（4.4 下标缩放恒折 shl）。
//   ③ 尾 `nop`（§6.1 对齐豁免）。
// p167 调用点差集 CLEAR（8/8）。结论：**封顶，无改动**（语义完成）。
// [GDataWriter 2026-09-11 重开 · 槽位大幅对齐] 按 P212/P309「函数头声明序控标量槽」
// 把 5 个标量前置声明（maxfd,nUnusedA,recvLen,nUnusedB,nfds）+ 聚合（buf/readfds/
// timeout/sockets）后置，并补 P171 两个 dead 标量（nUnusedA=1/nUnusedB=0，赋值为独立
// 语句）。实测槽位：nUnusedA -0x38✓ / nUnusedB -0x30✓ / recvLen -0x34✓ / nfds -0x2c✓ /
// timeout -0x44✓ / maxfd -0x3c✓(对齐)；**余 sockets(-0x48)↔maxfd(-0x3c) 互换**（第 2 试
// 把 sockets 提前声明 ⇒ timeval 反被顶到 -0x48，更劣，已回滚）= 4.4 聚合优先占低槽
// （N82附）不可翻。读数 137/138 不变（余 1 条 = FD_ZERO scaled 直存，N125/N81）。
void UdpThread::dispatch(void* arg)
{
    (void)arg;
    // [GDataWriter 2026-09-11] 槽位重排试验（P212/P309：函数头声明序控标量槽向）：
    // ORIG 标量序（深→浅）= maxfd(-0x48) / nUnusedA(-0x38) / recvLen(-0x34) /
    // nUnusedB(-0x30) / nfds(-0x2c)；旧声明序产出 sockets(-0x48)/maxfd(-0x3c) 互换。
    // 标量全部前置声明（先声明者占深槽），聚合（buf/readfds/timeout/sockets）后置。
    // [GDataWriter 2026-09-11 第 2 试负结论] 把数组 sockets 提到 maxfd 之前声明
    // ⇒ timeval 反被顶到 -0x48（`movl $0x1,-0x48`），更劣。4.4 聚合优先占低槽
    // （N82附）不可由声明序翻 ⇒ sockets(-0x48)↔maxfd(-0x3c) 互换封顶。
    // [K8GlobalData 2026-09-11 试验] 现状残差=纯槽位互换：ORIG sockets[0]/nUnusedA
    // 在 -0x3c/-0x38、maxfd 在 -0x48；我方 sockets[0]/nUnusedA 在 -0x48/-0x44、
    // maxfd 在 -0x3c ⇒ 把 maxfd 移到最后声明，观察是否整块互换归位。
    int nUnusedA;
    int recvLen;
    int nUnusedB;
    int nfds;
    char buf[0x1800];
    fd_set readfds;
    // [K8GlobalData 2026-09-11 试验②] 试①（maxfd 移末）已使 maxfd 归位 -0x48
    // 且 ndiff 26→20；剩 sockets(-0x44) 与 timeout(-0x40/-0x3c) 相对序与原码相反
    // （ORIG sockets@-0x3c 浅于 timeout@-0x44/-0x40）⇒ 交换 sockets/timeout 声明序。
    int sockets[1];
    struct timeval timeout;
    int maxfd;

    // [P171] ORIG 0x0857c9ea `movl $0x1,-0x38(%ebp)` 与 0x0857ca1a
    // `movl $0x0,-0x30(%ebp)`：两处只写不读的具名 dead 标量（全函数无第二
    // 处引用），照抄原码死写；补后栈槽整体下移 8，与 ORIG 帧内布局一致。
    nUnusedA = 1;
    sockets[0] = _bindUDPSocket((unsigned short)G_CEnvironment()->get_udp_port());
    g_bUdp_TH_start = true;
    nUnusedB = 0;
    while (!GlobalData::s_server_stop)
    {
        memset(buf, 0, sizeof(buf));
        // FD_ZERO(&readfds) 的 ORIG 4.1-glibc 展开形（逐元素循环；见函数头判词）。
        // [L8] 槽序：ORIG p(-0x24)/i(-0x28)；且 ORIG 尾测为 `cmpl $0x1f,-0x28;
        // setbe`（**无符号**比较）⇒ 计数变量须 unsigned（我方 int 版产 setle）。
        unsigned int fi;
        // [K8GlobalData 2026-09-11 · P291/P288 同 RecvPacket 实证] ORIG 0x0857ca56
        // 循环体为 `mov -0x28(%ebp),%edx; mov -0x24(%ebp),%eax; movl $0x0,(%eax,%edx,4)`
        // （scaled 直存，3 条）；旧 `int* fdBits = (int*)&readfds` 视角恒折
        // `shl $2; add`（4 条）⇒ 改具名 `fd_set* fds` + `__FDS_BITS(fds)[fi]`。
        fd_set* fds = &readfds;
        for (fi = 0; fi <= 0x1f; ++fi)
            __FDS_BITS(fds)[fi] = 0;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        maxfd = 0;
        for (int i = 0; i <= 0; ++i)
        {
            // [L8] ORIG 0x0857ca90 段 `mov -0x3c(%ebp,%eax,4),%eax; shr $0x5,%eax`
            // 为**逻辑**右移（无 `lea 0x1f/test/cmovs` 修正）⇒ FD_SET 的实参须按
            // unsigned 参与 `__FDELT(d)=d/32` 折叠；而 max 侧按 int 左值传址
            // （0x0857cac6 `lea -0x3c(%ebp)`）⇒ 各处分别 cast。
            FD_SET((unsigned int)sockets[i], &readfds);
            maxfd = std::max(maxfd, sockets[i]);
        }
        maxfd += 1;
        nfds = select(maxfd, &readfds, NULL, NULL, &timeout);
        if (nfds < 0)
        {
            continue;
        }
        for (int i = 0; i <= 0; ++i)
        {
            if (FD_ISSET((unsigned int)sockets[i], &readfds))
            {
                recvLen = recv(sockets[i], buf, sizeof(buf), 0);
                if (recvLen > 0)
                {
                    _processUDPData(buf, recvLen);
                }
            }
        }
    }
}