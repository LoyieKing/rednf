// ============================================================================
// DB_DeleteCharac::SendResult @0x084034fe
// StreamPool::Acquire("DBThread.cpp",0x13b0) → guard<<userIdx<<slot →
// put_binary(sig,0x14) → MsgQueueMgr::put(QUEUE_IDX 1)。恒返 1。
// ============================================================================
bool DB_DeleteCharac::SendResult(int userIdx, int slot, SIG_DELETE_CHARAC* sig)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp",
                                                           0x13b0), true);
    **guard << userIdx;
    **guard << slot;
    guard->put_binary(sig, 0x14);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    return true;
}
