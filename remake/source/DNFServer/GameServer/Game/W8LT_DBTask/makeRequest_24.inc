void DB_SecuServiceUpdateValidityTime::makeRequest(unsigned int accId,
                                                   unsigned int type,
                                                   unsigned short flag,
                                                   char* uid)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x88d4), true);
    guard.operator*()->operator<<((int)0x1c9);
    guard.operator*()->operator<<((int)-1);
    SIG_SECURITY_CARD_SERVICE_VALIDITYTIME* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_SERVICE_VALIDITYTIME>();
    sig->m_mId = accId;
    sig->m_type = type;
    sig->m_flag = flag;
    strncpy(sig->m_uid, uid, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
