void DB_UpdateGMData::makeRequest(bool flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8761), true);
    guard.operator*()->operator<<((int)0x14a);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((bool)flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
