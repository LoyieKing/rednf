// ============================================================================
// DB_PassPadUpdateFailCnt::makeRequest ORIG @ 0x08423eb4
//   opcode 0xa4 | -1，随后在 GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>
//   缓冲写入：+0x00 param_1(accId)、+0x04 param_2(failCnt)、
//   +0x08 strncpy(param_3, 0x10 webAddr)。ORIG 由 dispatch @0x08423bc4 如此消费
//   （+0 m_id、+4 fail_cnt、+8 ip_info）。
// ============================================================================
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
}