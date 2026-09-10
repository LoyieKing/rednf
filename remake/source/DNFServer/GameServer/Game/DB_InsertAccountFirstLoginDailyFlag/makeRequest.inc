void DB_InsertAccountFirstLoginDailyFlag::makeRequest(int p1, unsigned int p2,
                                                      unsigned int p3, int p4)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x953d), true);
    guard.operator*()->operator<<(0x342);
    guard.operator*()->operator<<(p1);
    guard.operator*()->operator<<(p2);
    guard.operator*()->operator<<(p3);
    guard.operator*()->operator<<(p4);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
