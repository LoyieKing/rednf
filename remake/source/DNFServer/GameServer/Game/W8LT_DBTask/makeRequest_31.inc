void DB_Check3rdPartyConcent::makeRequest(int uid, unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xada1), true);
    guard.operator*()->operator<<((int)0x2b7);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
