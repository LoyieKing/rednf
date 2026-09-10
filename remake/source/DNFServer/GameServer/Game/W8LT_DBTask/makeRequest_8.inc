void DB_LoadBloodBestRecord::makeRequest(int userIdx, unsigned int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x78c6), true);
    guard.operator*()->operator<<((int)0x13f);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
