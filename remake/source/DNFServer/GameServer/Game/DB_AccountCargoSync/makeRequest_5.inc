void DB_PassPadUpdateFailCnt::makeRequest(unsigned int param_1,
                                          unsigned int param_2, char* param_3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5f31), true);
    guard.operator*()->operator<<(0xa4);                  // SIG_SECURITY_CARD_UPDATE_FAIL_CNT
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    sig->m_mId = param_1;   // accId (m_id)
    sig->m_cnt = param_2;   // failCnt
    strncpy(sig->m_webAddr, param_3, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
