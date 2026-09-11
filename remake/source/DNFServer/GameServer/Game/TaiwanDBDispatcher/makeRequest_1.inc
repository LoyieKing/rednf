// ORIG 0x0815b89a（static）：Acquire(file, 0x1b9) → CStreamGuard(true) →
// internal_stream(guard, 0x8, a) → GetInBuffer<Taiwan::SigUpdateExitTime> 非空
// 时 init()、internal_stream 第三参传 b（ORIG 0xc(%ebp) 实证）、set(a, c)、
// MsgQueueMgr::put(DB_Q, guard)。
void DBUpdateExitTime::makeRequest(unsigned int a, int b, unsigned int c)
{
    // P89：单一视图表达式（合并 stream 局部）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x1b9), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x8, b);
    Taiwan::SigUpdateExitTime* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigUpdateExitTime>();
    if (sig != 0)
    {
        sig->init();
        sig->set(a, c);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}
