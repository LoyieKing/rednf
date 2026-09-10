int TimerDungeonDataStatistic::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x0863674a-0x08636757：Acquire 免槽直落 ctor；guard 全程作用域
    // （dtor 在 updateDataIntoDB/Regist 之后，catch @0fc）；写流走显式
    // operator*() 代理链；GetInBuffer 经 operator->()（ORIG 0x086367a2 ptEv call）
    register int ret;   // [b28d-solo] 共享尾 %ebx（ORIG 单出口）
    {
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0xb1a), true);
    guard.operator*()->operator<<((int)0xaf);
    guard.operator*()->operator<<((int)-1);
    SIG_DUNGEON_DATA_STATISTIC* stat =
        guard.operator->()->GetInBuffer<SIG_DUNGEON_DATA_STATISTIC>();
    memset(stat, 0, 0x80);
    *(int*)stat = 3;
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
