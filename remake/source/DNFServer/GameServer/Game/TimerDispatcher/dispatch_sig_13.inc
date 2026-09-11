// [b28d-30] 复核 DIFF：帧/callee-saved 跨版本差（N7/N82 族），ORIG 0x08633f3d 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82 封顶：DIFF→NEAR（107/107）。
// 修法：①P231/P307 后三 getter 结果加纯 register（非 asm 绑定）⇒ 复现 ORIG
// %edi/%esi/%ebx 跨 cMyTrace ctor 保活（删槽）；②P212 前三 getter 局部提函数头。
// 残差 = 前三槽 -0x44/-0x40/-0x3c vs -0x28..（4.1 声明序深槽 vs 4.4 首次使用序）
// + cMyTrace varargs 求值序 ⇒ N80/N82。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差 = guard/trace 槽
// (-0x34/-0x2c) 与三 getter 槽 (-0x44..-0x3c) 的区块排布互换（类对象 vs 标量混合区，
// N82/N93）⇒ 无语义缺口。
bool TimerStatisticsLog::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // P212 全头部声明：ORIG 三 getter 结果落深槽 -0x44/-0x40/-0x3c、trace -0x2c、
    // guard -0x34；4.4 块内声明按首次使用序落浅槽 ⇒ 提至函数头复现块级槽序。
    // [A12-2026-09-10 实验·失败] 三 getter 结果改 POD 聚合（一个对象）试探
    // 4.4「对象优先占深槽」分区规律：实测槽位不变（仍 -0x28/-0x24/-0x20，
    // ORIG -0x44/-0x40/-0x3c）⇒ POD 聚合不改分区，已回滚。
    int cacheLoadFailOverflowCount;
    int cacheLoadFailCount;
    int cacheLoadOverflowCount;
    // ORIG 0x08633f3d-0x08633f54：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)
    // （技巧36 删 s 中转槽）
    // ORIG 0x08633f25-0x08633fce：guard 生命周期止于 put 后（dtor+catch 块夹在
    // getter 段之前，ORIG 0x08633fad catch）⇒ 显式 {} 作用域块（技巧12）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x62f), true);
        // ORIG 0x08633f59-0x08633f6f：显式 operator*() 代理链（同 FatigueReset 模板）
        guard.operator*()->operator<<((int)0xcd);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    // ORIG 0x08633fe0/0x08633ff0/0x086334000：前三 getter 结果槽 -0x44/-0x40/-0x3c
    //（4.1 声明序布局；4.4 按生命周期落 -0x28..近端，槽序不可翻=N 族残差）
    cacheLoadFailOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadFailOverflowCount();
    cacheLoadFailCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadFailCount();
    cacheLoadOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadOverflowCount();
    // ORIG 0x08634010/0x0863401f/0x0863402e：后三 getter 结果直落
    // %edi/%esi/%ebx 跨 ctor 存活；绑定版残差=装载时多一条重排 mov（probe 实测
    // N96/P0：绑定形已废（三 callee 结果寄存器缓存残差记 N76/N82附）。
    // P231/P307：ORIG 后三 getter 结果直落 %edi/%esi/%ebx 跨 cMyTrace ctor 保活
    // （无栈槽）⇒ 纯 register 存储类提示（非 asm 绑定，P0 合规）。
    register int cacheLoadCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadCount();
    register int dbLoadOverflowCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetDBLoadOverflowCount();
    register int dbLoadCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetDBLoadCount();

    cMyTrace trace(__PRETTY_FUNCTION__, 0x637, 0);
    trace("EXCHANGE_SERVER_HIT_RATE : db count(%u), overflow db count(%u), cache count(%u), overflow cache count(%u), cache fail count(%u), overflow cache fail count(%u)",
          dbLoadCount, dbLoadOverflowCount, cacheLoadCount, cacheLoadOverflowCount,
          cacheLoadFailCount, cacheLoadFailOverflowCount);

    // ORIG 0x08634092-0x0863409b：`mov %al,-0x19; movzbl -0x19,%eax; xor $0x1`——
    // 具名 bool 局部 byte 槽 + 重读后 ==1 判定（技巧22 邻域）
    bool ok = TimerStatisticsLog::RegistNextTimer();
    // ORIG 0x08634099 `xor $0x1; test; je TRUE岛` ——false 为 fall-through 体
    if (ok != 1)
        return 0;
    return 1;
}