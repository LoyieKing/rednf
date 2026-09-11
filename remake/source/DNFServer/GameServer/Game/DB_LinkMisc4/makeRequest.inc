// ============================================================================
// DB_EventGrowthWeaponReset::makeRequest @ 0x08449980
// mangled _ZN25DB_EventGrowthWeaponReset11makeRequestEj
// Acquire("DBThread.cpp", 0xb95e)；<<0x316(int) → <<-1 → <<accId(u32 0x080e2292)
// ============================================================================
void DB_EventGrowthWeaponReset::makeRequest(unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb95e), true);
    guard.operator*()->operator<<(0x316);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}