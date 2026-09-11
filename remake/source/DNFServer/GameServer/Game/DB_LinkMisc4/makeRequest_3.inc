// ============================================================================
// DB_TestTradeRestrict::makeRequest @ 0x0844a132
// mangled _ZN20DB_TestTradeRestrict11makeRequestEj
// Acquire("DBThread.cpp", 0xba73)；<<0x31c(int) → <<-1 → <<accId(u32)
// ============================================================================
void DB_TestTradeRestrict::makeRequest(unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xba73), true);
    guard.operator*()->operator<<(0x31c);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}