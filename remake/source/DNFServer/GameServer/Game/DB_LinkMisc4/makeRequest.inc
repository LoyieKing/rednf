void DB_EventGrowthWeaponReset::makeRequest(unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb95e), true);
    guard.operator*()->operator<<(0x316);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
