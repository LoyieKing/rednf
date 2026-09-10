void DB_SecuServiceReward::makeRequest(unsigned int accId, char flag)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x847c), true);
    guard.operator*()->operator<<((int)0x19d);
    guard.operator*()->operator<<((int)-1);
    SIG_SECU_SERVICE_REWARD* sig =
        guard->GetInBuffer<SIG_SECU_SERVICE_REWARD>();
    sig->m_mId = accId;
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
