void DB_EventUpdateGrowthWeaponCharac::makeRequest(unsigned int accId,
                                                   unsigned int characNo,
                                                   int addInfo, short serverGroup)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb914), true);
    guard.operator*()->operator<<(0x314);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);       // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(characNo);    // operator<<(unsigned int) 0x080e2292
    guard.operator*()->operator<<(addInfo);     // operator<<(int) 0x080c8c56
    guard.operator*()->operator<<(serverGroup); // operator<<(short) 0x0822ad66
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
