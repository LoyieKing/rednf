// ============================================================================
// DB_PassPadUpdateCancelCnt::makeRequest ORIG @ 0x0842403a
//   opcode 0xa5 | -1，随后在 GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>
//   缓冲写入：+0x00 param_1(accId)、+0x04 param_2(cancelCnt)、
//   +0x08 strncpy(param_3, 0x10 webAddr)。
// ============================================================================
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
}