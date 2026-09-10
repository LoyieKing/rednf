void DB_PassPadUpdateCancelCnt::makeRequest(unsigned int param_1,
                                            unsigned int param_2,
                                            char* param_3)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5f5e), true);
    guard.operator*()->operator<<(0xa5);                  // SIG_SECURITY_CARD_UPDATE_CANCEL_CNT
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    sig->m_mId = param_1;   // accId
    sig->m_cnt = param_2;   // cancelCnt
    strncpy(sig->m_webAddr, param_3, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
