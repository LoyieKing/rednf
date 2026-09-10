void DB_ArrangeUserLoginInfo::makeRequest(unsigned int accId, bool flag,
                                          unsigned int uid)
{
    if (accId == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6ce9), true);
    guard.operator*()->operator<<((int)0xee);
    guard.operator*()->operator<<((int)-1);
    SIG_ARRANGE_USER_LOGIN_INFO* sig =
        guard->GetInBuffer<SIG_ARRANGE_USER_LOGIN_INFO>();
    sig->m_accId = accId;
    sig->m_flag = flag ? (char)1 : (char)0;
    sig->m_uid = uid;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
