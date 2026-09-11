// ORIG 0x08421c8e：同 DB_LastMsgDb 形态，Acquire("DBThread.cpp",0x59f7) →
// CStreamGuard(stream,true) → <<0x97(int) → <<stream(int)（原始 Stream* 指针值）
// → MsgQueueMgr::put(DISPATCH_Q=1,guard) → 返 1。无基类 dispatch 前置检查。
bool DB_LastMsgLogDb::dispatch(int userIdx, int slot, Stream* stream)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x59f7), true);
    **guard << 0x97;
    **guard << slot;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}