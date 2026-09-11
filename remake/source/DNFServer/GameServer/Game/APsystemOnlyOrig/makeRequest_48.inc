void DB_SecurityCardAddCertCnt::makeRequest(unsigned int accId)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8199), true);
    guard.operator*()->operator<<(0x17c);
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_ADD_CERT_CNT* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_ADD_CERT_CNT>();
    sig->m_mId = accId;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}