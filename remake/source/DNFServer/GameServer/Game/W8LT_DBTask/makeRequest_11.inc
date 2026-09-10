void DB_LoadEventLevelupSupport::makeRequest(int param1, int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xc2a0), true);
    guard.operator*()->operator<<((int)0x350);
    guard.operator*()->operator<<((int)param1);
    guard.operator*()->operator<<((int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
