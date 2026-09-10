void DB_UpdateGoblinPadAdvicePwdRewardTime::makeRequest(int param1,
                                                        unsigned int param2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x9927), true);
    guard.operator*()->operator<<((int)0x247);
    guard.operator*()->operator<<((int)param1);
    guard.operator*()->operator<<((unsigned int)param2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
