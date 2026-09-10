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
