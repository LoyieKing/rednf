// ORIG 0x0843c138 sz 0xec（_ZN28DB_ReqQueryCharacInfoMailbox11makeRequestEiPKc，
// 报告 DB_ReqQueryCharacInfoMailbox/makeRequest.md 全文转写）：
// Acquire("DBThread.cpp",0x8ec5) → CStreamGuard(stream,true)
//   → <<0x1f6(int) → <<userIdx(int) → GetInBuffer<SIG_QUERY_CHARAC_INFO>
//   → strncpy(name,0x1d) → put(DB_Q(2))。Dispatchers.cpp:16783 调用。
void DB_ReqQueryCharacInfoMailbox::makeRequest(int userIdx, const char* name)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8ec5), true);
    guard.operator*()->operator<<((int)0x1f6);
    guard.operator*()->operator<<((int)userIdx);
    SIG_QUERY_CHARAC_INFO* sig = guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    strncpy(sig->m_pad, name, 0x1d);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}