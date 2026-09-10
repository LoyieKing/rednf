bool DB_LastMsgLogDb::dispatch(int userIdx, int slot, Stream* stream)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x59f7), true);
    **guard << 0x97;
    **guard << slot;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
