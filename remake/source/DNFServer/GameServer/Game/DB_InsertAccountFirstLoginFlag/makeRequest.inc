void DB_InsertAccountFirstLoginFlag::makeRequest(int p1, unsigned int p2, int p3,
                                                 unsigned int p4, int p5)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9503), true);
    guard.operator*()->operator<<(0x25e);
    guard.operator*()->operator<<(p1);
    guard.operator*()->operator<<(p2);
    guard.operator*()->operator<<(p3);
    guard.operator*()->operator<<(p4);
    guard.operator*()->operator<<(p5);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
