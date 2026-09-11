bool WongWork::CWebEvent::ReqSaveEventItem(unsigned int p1, unsigned int p2,
                                           unsigned int p3, unsigned int p4,
                                           unsigned int p5, unsigned int p6,
                                           bool last)
{
    // ORIG T 0x086c1910 (size 0x11d)：Acquire 流 → <<0x4c(SIG) → <<-1 →
    // GetInBuffer<SIG_SAVE_EVENTITEM> 按 asm 偏移填参 → MsgQueueMgr put(DB_Q)
    // → guard 析构 → return 1。
    // ORIG 实参内联（Acquire 结果直传构造，无 stream 局部槽）；bool 直写
    // 字节（movzbl+store，P136）。
    CStreamGuard guard(s_stream_pool->Acquire("WebEvent.cpp", 0xa0), true);
    guard.operator*()->operator<<(0x4c);
    guard.operator*()->operator<<(-1);
    SIG_SAVE_EVENTITEM* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_EVENTITEM>();
    sig->m_field00 = p1;
    sig->m_field04 = p2;
    sig->m_field0c = p3;
    sig->m_field10 = p4;
    sig->m_field08 = p5;
    sig->m_field14 = p6;
    sig->m_field18 = last;
    s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}