void DB_DeleteAccountCargo::makeRequest(int param_1, unsigned int param_2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8c38), true);
    guard.operator*()->operator<<(0x1f0);                 // SIG_DELETE_ACCOUNT_CARGO
    guard.operator*()->operator<<(param_1);
    guard.operator*()->operator<<(param_2);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
