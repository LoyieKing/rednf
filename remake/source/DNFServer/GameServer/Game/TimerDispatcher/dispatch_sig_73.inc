// ORIG T 0x0863672a (_ZN25TimerDungeonDataStatistic12dispatch_sigEiij)
// [b28d-30] 复核 DIFF：N104 EH 尾块相对序（unwind 块与正常尾块互换、内容全等），ORIG 0x0863672a。
// [b28d-solo-TimerDispatcher] 判词 N104 封顶：ORIG unwind 块居中、正常 D1 尾块最后；
// 内容逐条全等，仅两尾块互换（4.4 恒把 epilogue 就地内联）。
bool TimerDungeonDataStatistic::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x0863674a-0x08636757：Acquire 免槽直落 ctor；guard 全程作用域
    // （dtor 在 updateDataIntoDB/Regist 之后，catch @0fc）；写流走显式
    // operator*() 代理链；GetInBuffer 经 operator->()（ORIG 0x086367a2 ptEv call）
    // [P7HeaderAxis 2026-09-11] bool 化（ORIG 整族 dispatch_sig 返回 bool）：ORIG 尾 `mov %ebx,%eax` 直返、无 test/setne。
    register bool ret;   // [b28d-solo] 共享尾 %ebx（ORIG 单出口）
    {
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0xb1a), true);
    guard.operator*()->operator<<((int)0xaf);
    guard.operator*()->operator<<((int)-1);
    SIG_DUNGEON_DATA_STATISTIC* stat =
        guard.operator->()->GetInBuffer<SIG_DUNGEON_DATA_STATISTIC>();
    memset(stat, 0, 0x80);
    stat->m_type = 3;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    GetInstanceVendingMachineStatistic()->updateDataIntoDB();
    // ORIG 0x0863680a-0x08636824：Regist byte 槽 -0x9 物化 + `xor $0x1; test; je`
    // 双岛直存 %ebx 跨 dtor（绑定消 bool byte 槽），单出口 return %ebx
    bool ok = RegistNextTimer();
    if (ok != 1)
        ret = false;
    else
        ret = true;
    }
    return ret;
}