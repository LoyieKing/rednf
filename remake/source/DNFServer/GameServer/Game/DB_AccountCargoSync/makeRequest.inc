void DB_CreateAccountCargo::makeRequest(int param_1, unsigned int param_2,
                                        unsigned int param_3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8bef), true);
    guard.operator*()->operator<<(0x1ee);                 // SIG_CREATE_ACCOUNT_CARGO
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    guard.operator*()->operator<<(param_3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
