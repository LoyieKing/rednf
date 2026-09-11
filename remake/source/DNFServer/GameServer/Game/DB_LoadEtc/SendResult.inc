// ============================================================================
// DB_LoadEtc::SendResult @0x08408766
// BigStreamPool::Acquire → guard<<userIdx<<slot→put_binary(sig,0x14b60) →
// MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)。
// ============================================================================
bool DB_LoadEtc::SendResult(int userIdx, int slot, SIG_LOAD_ETC* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x1f92),
                       true);
    guard.operator*()->operator<<(userIdx);      // op*(guard)→op<<(int)
    guard.operator*()->operator<<(slot);         // op*(guard)→op<<(int)
    guard.operator->()->put_binary(sig, 0x14b60);   // op->(guard)→put_binary
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    return true;                                 // ORIG ebx=1 恒定
}