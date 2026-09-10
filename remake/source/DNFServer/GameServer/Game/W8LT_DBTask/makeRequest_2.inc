void DB_LoadAccountCargo::makeRequest(int userIdx, unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c75), true);
    guard.operator*()->operator<<((int)0x1f2);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((unsigned int)accId);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
