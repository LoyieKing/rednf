// ============================================================================
// DB_UpdateInGameEventHistory::makeRequest @ 0x08444ac4
// mangled _ZN27DB_UpdateInGameEventHistory11makeRequestEjiii
// ============================================================================
void DB_UpdateInGameEventHistory::makeRequest(unsigned int accountId,
                                              int eventType, int history1,
                                              int history2)
{
    // ORIG：Acquire 返回值直接作为 CStreamGuard 构造实参（无中间变量）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xaba8), true);
    guard.operator*()->operator<<(0x2ad);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accountId);
    guard.operator*()->operator<<(eventType);
    guard.operator*()->operator<<(history1);
    guard.operator*()->operator<<(history2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(,2,)
}