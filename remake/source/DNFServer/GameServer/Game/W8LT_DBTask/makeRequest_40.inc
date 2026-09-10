void DB_LoadPuUserCheck::makeRequest(unsigned int param1, int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xbcc2), true);
    guard.operator*()->operator<<((int)0x2e1);
    guard.operator*()->operator<<((int)param2);
    guard.operator*()->operator<<((unsigned int)param1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
