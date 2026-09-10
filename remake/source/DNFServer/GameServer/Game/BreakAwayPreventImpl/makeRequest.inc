void DB_UpdateBreakAwayUvList::makeRequest(unsigned int accId, char* uvInfo)
{
    // [b28c-31] ORIG 0x0843874x：Acquire 结果寄存器直传 CStreamGuard::C1（无栈槽往返，
    // P0/P89）⇒ 实参内联，勿命名 stream 局部。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8622), true);
    guard.operator*()->operator<<(0x147);             // opcode（DBThread 分发表）
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);
    SIG_BREAK_AWAY_UV_LIST* buf =
        guard.operator->()->GetInBuffer<SIG_BREAK_AWAY_UV_LIST>();
    strncpy(buf->m_pad, uvInfo, 0x1f);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
