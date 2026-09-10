void DB_GetUserRegdate::makeRequest(int uid, unsigned int accId)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4b2), true);
    guard.operator*()->operator<<(0x2a1);
    guard.operator*()->operator<<(uid);
    SIG_GET_USER_REGDATE* sig =
        guard.operator->()->GetInBuffer<SIG_GET_USER_REGDATE>();
    sig->m_id = accId;
    sig->m_regDate = 0;
    sig->m_regDay = 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
