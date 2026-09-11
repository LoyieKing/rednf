// ORIG 0x080c8616：BigStreamPool 序列化 → put DB_Q（QUEUE_IDX 2）。
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
    // [M3S4 实测-2形 · 87=88-1 封顶] ①unsigned 视角（本形）与 ②具名成员
    // `sig->m_field0 = pUser->get_charac_no(-1)`（SigTypes_A.h:489 int 字段）读数
    // 与产物逐行相同 ⇒ 残差 1 条在 ORIG 独有的槽重载 `mov -0xc(%ebp),%eax;
    // mov %edx,(%eax)`：4.4 改用 callee-saved %ebx 保活（call 前 `mov -0xc(%ebp),
    // %ebx`，call 后 `mov %eax,(%ebx)`）⇒ 跨版本 regalloc 差，无源码形可翻。
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(sig)) =
        pUser->get_charac_no(-1);   // sig->characNo @+0
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}