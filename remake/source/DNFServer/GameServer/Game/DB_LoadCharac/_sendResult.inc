// ============================================================================
// DB_LoadCharac::_sendResult @0x0840a4ce
// StreamPool::Acquire("DBThread.cpp",0x233a) → guard<<7<<slot →
// guard->GetInBuffer<SIG_LOAD_CHARAC>() → memcpy(sig,0x703) →
// MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)；随后 cMyTrace 0x2347
// "CharLastPlayTick char(%d) t(%d)"（charac_no=+4 / t=+0x59）。
// ============================================================================
void DB_LoadCharac::_sendResult(int userIdx, int slot, SIG_LOAD_CHARAC* sig)
{
    // [b28d-64] P259/P0：ORIG 0x0840a4e0 `call Acquire; mov %eax,%ebx;
    // mov %ebx,0x4(%esp)` 寄存器中转（无 stream 具名槽 -0x10）⇒ 内联实参。
    // [b28d-solo] 块作用域修正：ORIG 的 CStreamGuard::D1 在函数最末（trace 之后）
    // ⇒ 原码 trace 也在 guard 作用域内（Ghidra C 的 D1 调用位于 op() 之后）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x233a), true);
    guard.operator*()->operator<<(7);
    guard.operator*()->operator<<(slot);
    SIG_LOAD_CHARAC* out = guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC>();
    memcpy(out, sig, 0x703);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);

    // [b28d-64] P227：ORIG 0x0840a5ac 两实参先入 %esi/%ebx 跨 cMyTrace ctor
    // 保活（`mov 0x14(%ebp),%eax; mov 0x59(%eax),%esi; …%ebx`）⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x2347, 0)(
        "CharLastPlayTick char(%d) t(%d)", sig->m_characNo,
        sig->m_characData.m_field4d);
}