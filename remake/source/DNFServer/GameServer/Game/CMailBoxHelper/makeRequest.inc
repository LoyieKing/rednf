void DB_GetDeathTowerBestRecord::makeRequest(int uid, unsigned int characNo)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6be0), true);
    guard.operator*()->operator<<(0xe2);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(characNo);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
