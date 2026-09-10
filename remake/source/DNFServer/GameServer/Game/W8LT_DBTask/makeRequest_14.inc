void DB_UpdatePvPGrade::makeRequest()
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6ac8), true);
    guard.operator*()->operator<<((int)0xd4);
    guard.operator*()->operator<<((int)-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
