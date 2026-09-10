bool BestClearTime::_saveData(CUser* pUser)
{
    // ORIG 无 stream 具名槽（P0/P259：Acquire 结果 %eax 直传 CStreamGuard C1
    // 第二实参；旧形 +1 store/load 且 guard 槽 -0x14→-0x1c）。
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("BestClearTime.cpp", 0x16),
        true);
    // ORIG 0x080c8616：`(*guard)->operator<<(x)`（operator* de() + operator<<）
    // 与 `guard->GetInBuffer`（operator-> pt()）标识调用形态，勿改为直接成员调用
    // （直接 `guard << x` 会省略 ORIG 的 de() 调用）。
    (*guard)->operator<<((int)0x300);
    (*guard)->operator<<((int)pUser->GetUID());

    SIG_BEST_CLEAR_TIME* sig = guard->GetInBuffer<SIG_BEST_CLEAR_TIME>();
    // ORIG：`cmpl $0,sig; jne body; mov $0,%ebx; jmp 尾部` + 尾 `mov $1,%ebx`
    // ⇒ 返回值跨析构保活于 ebx（无 bool 槽）：早退形（旧 `bool ok` 形落槽
    // -0x9 并令 guard 槽 -0x14→-0x18）。
    if (sig == 0)
    {
        return false;
    }
    memset(sig, 0, sizeof(SIG_BEST_CLEAR_TIME));
    // ORIG 虚调 vtable slot2 = getData(char*)。
    getData(reinterpret_cast<char*>(sig));
    // ORIG store 为 RHS-first + eax 中转（`mov %eax,%edx; mov -0xc(%ebp),%eax;
    // mov %edx,(%eax)`）⇒ 目标字段与 RHS 异型（P211 档③）⇒ unsigned 视角。
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig)) =
        pUser->get_charac_no(-1);   // sig->characNo @+0
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
