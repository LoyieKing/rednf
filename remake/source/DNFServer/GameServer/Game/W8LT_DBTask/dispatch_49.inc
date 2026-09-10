bool DB_LastMsgDb::dispatch(int userIdx, int slot, Stream* stream)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x59ed), true);
    **guard << 0x97;
    // ORIG 0x8421c3f：`mov 0x10(%ebp),%edx` = 形参 slot（非 stream 指针）。
    **guard << slot;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
