// ===========================================================================
// SendResult — 报告 @0x084027b0
// 大流包：userIdx、slot、SIG 全量 0x5354 二进制 → DISPATCH_Q。
// ===========================================================================
bool DB_CreateCharac::SendResult(int userIdx, int slot, SIG_CREATE_CHARAC* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x11db),
                       true);
    guard.operator*()->operator<<(userIdx);       // op*(guard)→op<<(int)
    guard.operator*()->operator<<(slot);          // op*(guard)→op<<(int)
    guard.operator->()->put_binary(sig, 0x5354);  // op->(guard)→put_binary
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}