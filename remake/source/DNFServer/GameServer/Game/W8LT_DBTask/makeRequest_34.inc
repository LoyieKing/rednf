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
