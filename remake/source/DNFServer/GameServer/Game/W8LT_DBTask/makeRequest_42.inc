void DB_UpdateOntimeLastRecvIdx::makeRequest(int param1, unsigned int param2,
                                             int param3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 41999), true);
    guard.operator*()->operator<<((int)0x297);
    guard.operator*()->operator<<((int)param1);
    guard.operator*()->operator<<((unsigned int)param2);
    guard.operator*()->operator<<((int)param3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
