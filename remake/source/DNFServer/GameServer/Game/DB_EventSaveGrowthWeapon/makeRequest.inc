void DB_EventSaveGrowthWeapon::makeRequest(unsigned int accId, short serverGroup)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb8ef), true);
    guard.operator*()->operator<<(0x313);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(serverGroup); // operator<<(short) 0x0822ad66
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
