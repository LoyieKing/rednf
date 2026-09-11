// ---- SavePrivateStore（ORIG T 0x85ca116，报告 SavePrivateStore.md）----
// Stream(0x77)+GetUID 后取 SIG_PRIVATE_STORE：清零 0x434、写 charac_no、
// 拷贝 14 条目至 +8、按 isDollCreated 写 +0x430 标志，入 DB 队列(DB_Q=2)。
// [b28d-24] DIFF 残54: P167 MISSING CStreamGuard::deEv×2 + isDollCreated×1；EXTRA ptEv×2（`*guard` vs `guard->` 链，P103/P132）。ORIG 0x085ca116
// [b28d-solo-CPrivateStoreMgr] 残 3 行：P259 去 stream 具名局部（Acquire 直落
// CStreamGuard ctor 实参槽）。余 `&sig->m_items`（+0x8 常量偏移取址）ORIG
// `lea 0x8(%eax),%ebx` vs 4.4 `add $0x8,%eax; mov %eax,%ebx`——数组/取址/数组引用/
// 引用形 4 形实测均不可择（N107/N18a 族：4.4 成员常量偏移取址恒 add）。
void CPrivateStore::SavePrivateStore()
{
    // P259：ORIG 0x085ca116 无 stream 具名局部，Acquire 结果直落 ctor 实参槽。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PrivateStore.cpp", 0xbc6),
                       true);
    // ORIG P167：两次 `operator<<` 走 CStreamGuard::operator*（deEv）而非
    // operator->（ptEv），GetInBuffer 仍走 operator->（P103/P132 分工）。
    (*guard)->operator<<(0x77);
    (*guard)->operator<<((int)m_pSeller->GetUID());
    SIG_PRIVATE_STORE* sig = guard.operator->()->GetInBuffer<SIG_PRIVATE_STORE>();
    std::memset(static_cast<void*>(sig), 0, 0x434);
    sig->m_characNo = m_pSeller->get_charac_no(-1);
    // [b28d-solo] P311 成员数组视图：ORIG 0x85ca1e3 `lea 0x8(%eax),%ebx`（成员
    // 数组名退化取址单条 lea）；字节数组 + cast 形 4.4 恒 `add $0x8,%eax;
    // mov %eax,%ebx`（N107 六形探针之一）。
    struct SigItemsView
    {
        char m_pad[8];
        PrivateStoreItem m_items[14];
    };
    std::copy(m_items.begin(), m_items.begin() + 0xe,
              reinterpret_cast<SigItemsView*>(sig)->m_items);
    // ORIG 双守卫各重读一次 getter（P48/P184；三目形只出 1 次调用）。
    if (isDollCreated())
        sig->m_dollCreated = 1;
    else if (!isDollCreated())
        sig->m_dollCreated = 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

}