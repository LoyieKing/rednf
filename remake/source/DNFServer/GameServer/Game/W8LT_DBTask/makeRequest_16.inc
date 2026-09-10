void DB_LoadBreakAwayPreventData::makeRequest(bool flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8642), true);
    guard.operator*()->operator<<((int)0x141);
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((bool)flag);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
