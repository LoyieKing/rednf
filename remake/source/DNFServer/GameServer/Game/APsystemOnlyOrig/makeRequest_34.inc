// ============================================================================
// DB_ChangeLetterStat::makeRequest —— ORIG 0x084261fc（报告 makeRequest.md）
// Acquire("DBThread.cpp",0x6454) → CStreamGuard(stream,true) → <<userIdx →
// <<slot → GetInBuffer<SIG_LETTER_STAT>：m_characNo=characNo、
// m_mailIdx=mailIdx、m_state=(short)state → MsgQueueMgr::put(DISPATCH_Q)。
// ============================================================================
// [b28d-4] AE：AE/IDENTICAL：AE 归一化（@plt+nop 剥离+重定位归一）后逐条相等
void DB_ChangeLetterStat::makeRequest(int userIdx, int slot, int characNo,
                                      int mailIdx, int state)
{
    // [b26L17 翻案 N76→identical] m_mailIdx int→unsigned int（SigTypes_B.h，
    //   字段↔形参异型化 → 4.4 值先载=ORIG 0x14(%ebp) 形）；m_characNo 保持
    //   int（该行 ORIG 地址先，实测异型化反翻）。69/69 逐条相等。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6454), true);
    guard.operator*()->operator<<((int)userIdx);
    guard.operator*()->operator<<((int)slot);
    SIG_LETTER_STAT* sig = guard.operator->()->GetInBuffer<SIG_LETTER_STAT>();
    sig->m_characNo = characNo;
    sig->m_mailIdx = mailIdx;
    sig->m_state = (short)state;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}