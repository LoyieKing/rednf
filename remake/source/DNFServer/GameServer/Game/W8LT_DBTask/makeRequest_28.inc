void DB_SecurityCardDisuse::makeRequest(unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6e2d), true);
    guard.operator*()->operator<<((int)0xfe);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_DISUSE* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_DISUSE>();
    sig->m_mId = accId;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
