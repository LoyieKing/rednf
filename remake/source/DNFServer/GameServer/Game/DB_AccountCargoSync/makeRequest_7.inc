// ============================================================================
// DB_SecurityCardUpdateCancelCnt::makeRequest ORIG @ 0x0842c112
//   opcode 0x102 | -1，缓冲 +0x00 param_1(accId)、+0x04 param_2(cancelCnt)。
// ============================================================================
void DB_SecurityCardUpdateCancelCnt::makeRequest(unsigned int param_1,
                                                 unsigned int param_2)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6f25), true);
    guard.operator*()->operator<<(0x102);                 // SIG_SECURITY_CARD_UPDATE_CANCEL_CNT
    guard.operator*()->operator<<(-1);
    SIG_SECURITY_CARD_UPDATE_FAIL_CNT* sig =
        guard.operator->()->GetInBuffer<SIG_SECURITY_CARD_UPDATE_FAIL_CNT>();
    sig->m_mId = param_1;   // accId
    sig->m_cnt = param_2;   // cancelCnt
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}