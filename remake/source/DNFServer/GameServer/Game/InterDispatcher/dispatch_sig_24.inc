int Inter_WebNoticeIngameAdvertisement::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    // ORIG 同形：Acquire 返回值直接进 guard 实参。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("InterDispatcher.cpp", 0x2277), true);
    guard.operator*()->operator<<(0x2f8);
    guard.operator*()->operator<<(-1);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 0;
