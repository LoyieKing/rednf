// ---------------------------------------------------------------------------
// RegistServerSchedule() —— ORIG 0x82a0459（0x49b 字节）逐条转写。
// 全部为 G_TimerQueue()->InsertTimer(OBJ_TYPE=2, 0, msg, delaySec, 0, 0)。
// ---------------------------------------------------------------------------
// [b28d-28] DIFF | ORIG 0x082a0459 | N82：槽/寄存器分配差（大函数）。
// [b28d-solo-GameMain] N95/N82 封顶 | 235/242 | 真语义 bug 修：ORIG magic
// 0x88888889+`sar $0x4` = **÷30** ⇒ 源形 `p->tm_min % 30`（半点对齐；旧实现
// 误用 tm_mon/15）；0x55 delay 改内联实参（P24）；残差帧 0x74 vs 0x64 + 槽平移。
int RegistServerSchedule()
{
    // [Fn28Gap 登记 · MiscLane 落地 2026-09-11] ORIG 有 b 段符号
    // `_ZZ20RegistServerSchedulevE1i`（4B @0x0941f860，host 区间 0 引用 = 死 static）。
    // 探针 /tmp/lane_MiscLane/probe_static.cpp 实证：4.4.7 -O0 对**未使用**的函数级
    // `static int i;` 照样发射同名 b 4B 符号（零指令、零帧影响）⇒ 补回即消该仅ORIG。
    static int i;

    // 0x50 / 0x52：固定 0 秒
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x50, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x52, 0, 0, 0);

    // 0x54：对齐到整半点（ORIG：1740 - 60*(tm_mon - 30*(tm_mon/15)) + (60 - tm_sec)）
    // [N3Free171to190·GameMain] AE 修法（P339 块作用域=槽序）：ORIG 的 now/t/p/delay
    // 全在**函数体 toplevel**（ORIG 后端先分配 toplevel、后块内变量；4.4.7 相反）——
    // 去掉块后槽序 now(-0x1c)/t(-0x48)/p(-0x18)/delay(-0x14)/i(-0xc) 与 ORIG 全等
    // （235=235 AE；旧形 235/235 但 p/delay/i 三槽轮换）。改名 now54/p54/delay54 仅为
    // 避开 0x8e 段的 `delay`（局部名不入符号，mangled/行为不变）。
    std::time_t now54 = std::time(0);
    std::tm t;
    // P43：ORIG 0x082a04fb `mov %eax,-0x18; mov 0x4(%eax),%ecx` 经返回值指针读字段。
    std::tm* p54 = localtime_r(&now54, &t);
    // ORIG 0x082a0504 magic 0x88888889 + `sar $0x4` = **÷30**（sar 3 才是 ÷15），
    // 随后 `v = tm_min - 30*q` ⇒ 源形 `tm_min % 30`（半点对齐；旧实现误用
    // tm_mon/15，属真语义 bug）。v 无槽（寄存器直算），故内联进 delay 表达式。
    int delay54 = 1740 - 60 * (p54->tm_min % 30) + (60 - p54->tm_sec);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x54, delay54, 0, 0);

    // 0x55：TimerStatisticLevel 下一统计点距当前秒数（ORIG 0x082a0588：差在 %ebx
    // 跨 G_TimerQueue() 调用保活，无具名槽 ⇒ P24 内联实参）。
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x55,
        TimerStatisticLevel::CalcNextTime() - static_cast<int>(std::time(0)), 0, 0);

    // 0x59 / 0x5a / 0x5b / 0x61 / 0x66 / 0x89：ORIG 逐个独立 G_TimerQueue()->InsertTimer
    // （非循环；循环只发 1 个 call 点，P167 缺 5）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x59, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x5a, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x5b, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x61, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x66, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x89, 0, 0, 0);

    if (IsLightServer())
    {
        // 0x68：每条 light server notice 一条 0 秒定时器，param2=序号。
        // ORIG 读 CDataManager+0xa4 vector<stLightServerNotice_t> —「推断」布局。
        // P183：ORIG 0x82a0798/0x82a07a1 每轮迭代重新 `call G_CDataManager;
        // add $0xa4`（无地址具名缓存槽 -0x1c）⇒ 全表达式置于循环条件。
        for (unsigned int i = 0;
             i < reinterpret_cast<std::vector<stLightServerNotice_t>*>(
                     reinterpret_cast<char*>(G_CDataManager()) + 0xa4)->size();
             ++i)
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                        (TIMER_MESSAGE)0x68, 0, (int)i, 0);
    }

    TimerScheduleTenSecond::registNextTimer(10);

    // 0x8d：固定 0 秒
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x8d, 0, 0, 0);

    if (!G_GameWorld()->IsPVPChannel())
        Timer_DungeonInoutOpenTime::RegistNextTimer();

    // 0x8e：ORIG 0x82a0814 起 `call get_channel_no` → ÷10 商（魔数 0x66666667
    // + `sar $0x2`）→ `shl $2; add; add` = ×10 再 `sub` 取余 ⇒ 源形 `c % 10`；
    // c/取余全程寄存器，仅 delay 落 -0x10 槽（P0/P183：禁 c、m 具名槽）。
    int delay = 60 * (G_CEnvironment()->get_channel_no() % 10);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x8e, delay, 0, 0);

    // 0x9a：固定 0 秒（ORIG 0x82a08af，0x8e 之后注册）
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x9a, 0, 0, 0);

    // ORIG 0x82a08cf..0x82a08f3：尾部 checkContinus + loadServerState（全局
    // 0x941f7f0）。ORIG 逐次 `mov 0x941f7f0,%eax; mov %eax,(%esp); call`，**无**
    // test/je 判空（指令形双向直证）⇒ 删我方发明的 `!= 0` 防御（语义轴①）。
    GlobalData::s_serverStateManager_->checkContinus();
    GlobalData::s_serverStateManager_->loadServerState();
    // ORIG 0x82a08f6 `mov $0x1,%eax`（Ghidra `return 1`）⇒ 返回 int。
    return 1;
}