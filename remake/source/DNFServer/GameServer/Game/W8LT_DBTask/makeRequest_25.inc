void DB_SecurityCardUpdateValidityTime::makeRequest(unsigned int accId,
                                                    unsigned int flag)
{
    if (accId == 0)
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6ebb), true);
    guard.operator*()->operator<<((int)0x100);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_UPDATE_VALIDITYTIME* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_UPDATE_VALIDITYTIME>();
    sig->m_mId = accId;
    sig->m_flag = flag;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
