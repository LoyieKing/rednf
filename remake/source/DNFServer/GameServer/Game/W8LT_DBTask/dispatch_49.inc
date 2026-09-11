// ORIG 0x08421bcc：StreamPool::Acquire("DBThread.cpp",0x59ed) →
// CStreamGuard(stream,true) → <<0x97(int) → <<stream(int)（原始 Stream* 指针值）
// → MsgQueueMgr::put(DISPATCH_Q=1,guard) → 返 1。无基类 dispatch 前置检查。
bool DB_LastMsgDb::dispatch(int userIdx, int slot, Stream* stream)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x59ed), true);
    **guard << 0x97;
    // ORIG 0x8421c3f：`mov 0x10(%ebp),%edx` = 形参 slot（非 stream 指针）。
    **guard << slot;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}