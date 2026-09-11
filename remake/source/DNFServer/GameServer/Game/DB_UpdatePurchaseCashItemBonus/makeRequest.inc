// ============================================================================
// DB_UpdatePurchaseCashItemBonus::makeRequest @ 0x0843e560
// mangled _ZN30DB_UpdatePurchaseCashItemBonus11makeRequestEij
// ============================================================================
void DB_UpdatePurchaseCashItemBonus::makeRequest(int p1, unsigned int p2)
{
    // b28c-33：同 DB_UpdateBreakAwayAccruedCera::makeRequest，Acquire 返回值直传
    // guard ctor（ORIG 无 stream 局部、guard 槽 -0x10）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9415), true);
    guard.operator*()->operator<<(0x23c);
    guard.operator*()->operator<<(p1);
    guard.operator*()->operator<<(p2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}