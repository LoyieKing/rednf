void DB_SecurityCardUpdateFailCnt::makeRequest(unsigned int accId,
                                               unsigned int failCnt)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6f0b), true);
    guard.operator*()->operator<<(0x101);
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        guard->GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    sig->m_mId = accId;
    sig->m_cnt = failCnt;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}