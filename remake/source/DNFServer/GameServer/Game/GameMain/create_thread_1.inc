// create_thread() —— ORIG 0x829fd30（0x2f8 字节，逐指令转写）：
// 依次 begin 七个全局线程并 setCurrentPacketPool 绑定包池；任一失败返回 0；
// 尾部非 PvP 整合频道（且 channelType!=7）时再启动 exchange 线程。
// [b28d-28] DIFF | ORIG 0x0829fd30 | P167 CLEAR（28/28）；N82/P73：ORIG bool
// 局部 16× 槽往返（`mov %al,-0x9; movzbl -0x9`），4.4 寄存器化（202 vs 160）。
// [b28d-solo-GameMain] AE | 202/202 | p167 28/28 CLEAR | 修法：宏展开为 bool ok
// 槽形（14 处 `ok = begin()/setCurrentPacketPool(); if (!ok) return 0;`）+
// exchange 分支 2 处（BeginExchangeServerThread 返回类型 bool 由 Main 代落）。
bool create_thread()
{
    // ORIG 0x0829fd43 每步均为 `mov %al,-0x9(%ebp); movzbl -0x9; xor $1; test;
    // je`（P73/P248 档②）——bool 具名局部 ok 的槽往返，16 次复用同一槽 -0x9；
    // `if (!obj->begin())` 直测形（4.4 寄存器化）会少 42 条。
    bool ok;
    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_dispatch_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_NETWORK /*0*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_dispatch_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_udp_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_5 /*5*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_udp_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_monitor_tcp_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_6 /*6*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_monitor_tcp_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_7 /*7*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr_for_logDB)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_MONITOR_TCP /*8*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr_for_logDB)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_accept_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_DISPATCH /*1*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_accept_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_net_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_DB /*2*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_net_thr)->getThreadID());
    if (!ok) return 0;

    // ORIG 条件：!IsIntegratedPvPBaseChannel() && GetChannelType()!=7
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel()
        && G_GameWorld()->GetChannelType() != 7)
    {
        // ORIG 0x0829ffdb..0x082a0028：同样两步 ok 槽检查（各 4+8 条）。
        // BeginExchangeServerThread 返回类型 bool 已由 Main 代落（0x848f2b8 透传）。
        ok = exchange_server::BeginExchangeServerThread();
        if (!ok) return 0;
        ok = GlobalData::setCurrentPacketPool(
            THREAD_TYPE_TIMER /*3*/,
            exchange_server::getExchangeServerThreadID());
        if (!ok) return 0;
    }
    return 1;
}