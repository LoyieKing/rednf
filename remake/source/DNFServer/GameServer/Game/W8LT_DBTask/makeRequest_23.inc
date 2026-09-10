void DB_SaveCleanpadPoint::makeRequest(unsigned int point, int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7aaa), true);
    guard.operator*()->operator<<((int)0x154);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((unsigned int)point);
    guard.operator*()->operator<<((int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
