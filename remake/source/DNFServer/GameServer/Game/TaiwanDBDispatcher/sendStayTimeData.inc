void CheckStayTimeEvent::sendStayTimeData(int uid)
{
    // P89：单一视图表达式（合并 stream 局部）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x192), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)0x5, uid);
    Taiwan::SigStayTimeEvent* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigStayTimeEvent>();
    if (sig != 0)
    {
        // [b21] ORIG put @0x815b735 movl $0x1=DISPATCH_Q（QueueSweep21 回扫实证）；
        // 非 DB_Q(2)——同族 DB 派发器归队队列 idx 1。
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
}
