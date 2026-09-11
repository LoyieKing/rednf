// ---------------------------------------------------------------------------
// MainLoop() —— ORIG 0x829fa2b（0x305）：主循环每秒一轮。ORIG 局部为
// 周期/计数器组：-0x34 每 30 轮 UpdateChannelUser+状态日志；-0x40 每 10 轮
// 心跳（monitor/guild proxy 遍历，GetServerProxy(组号)）；-0x24 每 2 轮
// 翻转 CmmChannelProxy 上报；-0x1c 每 0x12c 轮向 DISPATCH_Q 投递心跳流包
// （Acquire("App.cpp", 0x2728) + << 0x30a << -1）。退出条件
// !GlobalData::s_server_stop（@0x940bd64），返回 1。
// ---------------------------------------------------------------------------
// SIGFIX:MainLoop —— ORIG __PRETTY_FUNCTION__ 串 "bool MainLoop()"；ORIG 尾
// 0x829fd24 `mov $0x1,%eax` 后 leave/ret（循环退出返回 1，P174）。
// [b28d-28] DIFF | ORIG 0x0829fa2b | P167 CLEAR（36/36）；N82：常量数组初始化槽
// （-0x48..-0x38）+ 帧 0x60 vs 0x50 + 槽平移。
// [b28d-solo-GameMain] AE | 190=190 | 真语义 bug 修 ×2：
// ①ORIG 0x0829fb02 `cmpl $0x9; setg; addl $1,-0x40; test; je` ⇒ 源形
//   `if (cnt10++ > 9) {…; cnt10=0;}`（旧形真分支后值 1≠ORIG 0）；
// ②心跳循环 ORIG -0x4c 是**独立计数器**（GetNextIndex(int&) 引用出参，初值 0，
//   cond `m_count > next`）；旧 `mi = GetNextIndex(mi)` 把 proxy 索引当计数器
//   （首轮跳 indexArray[1]）⇒ 漏发心跳。
// 修法：P52 链式单表达式 `mgr->GetServerProxy(mi)->SendHeartBeat(G_CEnvironment()
// ->get_channel_no())`（实参先物化 %ebx 跨 receiver 保活）+ 两循环复用同一 next
// 槽（guild 前复位 0，ORIG 同 -0x4c）+ P259 `Acquire(...)` 内联进 CStreamGuard
// ctor 实参（去 s 具名槽往返）+ 6 个 dead 局部复现帧头 12 条初始化。
// 帧 0x60 与 ORIG 全等，指令数 190=190。
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

    // [N3 2026-09-11 P139] ORIG 发射局部静态
    //   `_ZZ8MainLoopvE19__PRETTY_FUNCTION__` @0x08c1b058（size 0x10 = "bool MainLoop()"）
    //   但函数体内 **0 引用**（objdump [0x0829fa2b,0x829fd30] 搜 0x8c1b058 命中 0）⇒ 死引用，
    //   只补符号不产生任何指令（N8W8LT 四形探针实证 `sizeof` 形零指令且发射串）。
    // [N3Free171to190·GameMain 修正 2026-09-11] `(void)sizeof(...)` 形**不发射**该符号：
    //   sizeof 是编译期常量，静态数组未被 ODR-use ⇒ 我方 .o 仍缺
    //   `_ZZ8MainLoopvE19__PRETTY_FUNCTION__`（P8 的 prett2.py 对本 TU 报「缺口 1」即此）。
    //   实测有效形 = 不可达 `if (0)` 内 ODR-use：发射同名同 size(0x10) 符号且**零指令、零槽**
    //   （MainLoop 保持 AE 190=190，TU 四数 62/212/3/3 不变）。对照 `if (0) { cMyTrace(…)(…); }`
    //   会为临时对象占槽 ⇒ 帧 0x60→0x70、MainLoop AE→NEAR（已实测否决）。
    if (0) { (void)__PRETTY_FUNCTION__; }

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
}