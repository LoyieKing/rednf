// ============================================================================
// DB_LoadInventory::SendResult_forchina @0x0840d98c
// BigStreamPool::Acquire("DBThread.cpp",0x2939) → guard<<userIdx<<slot →
// put_binary(sig,0x1544c) → MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)。
// ============================================================================
bool DB_LoadInventory::SendResult_forchina(int userIdx, int slot,
                                           SIG_LOAD_INVENTORY* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x2939),
                       true);
    guard.operator*()->operator<<(userIdx);          // op*(guard)→op<<(int)
    guard.operator*()->operator<<(slot);             // op*(guard)→op<<(int)
    guard.operator->()->put_binary(sig, 0x1544c);    // op->(guard)→put_binary
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    return true;                                     // ORIG ebx=1 恒定
}