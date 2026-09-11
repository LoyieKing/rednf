// ============================================================================
// DB_EventUpdateGrowthWeaponTimepiece::makeRequest @ 0x084497ea
// mangled _ZN35DB_EventUpdateGrowthWeaponTimepiece11makeRequestEjsi
// Acquire("DBThread.cpp", 0xb941)；<<0x315 → <<-1 → <<accId(u32) →
// <<timepiece(short 0x0822ad66) → <<value(int 0x080c8c56)
// ============================================================================
void DB_EventUpdateGrowthWeaponTimepiece::makeRequest(
    unsigned int accId, short timepiece, int value)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb941), true);
    guard.operator*()->operator<<(0x315);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(timepiece);   // operator<<(short) 0x0822ad66
    guard.operator*()->operator<<(value);       // operator<<(int) 0x080c8c56
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}