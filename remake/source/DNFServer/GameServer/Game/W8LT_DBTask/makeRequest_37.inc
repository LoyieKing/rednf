void DB_UpdateRevengeData::makeRequest(unsigned int characNo,
                                       unsigned char attack,
                                       unsigned char revenge)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x98bd), true);
    guard.operator*()->operator<<((int)0x244);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)characNo);
    guard.operator*()->operator<<(attack);
    guard.operator*()->operator<<(revenge);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
