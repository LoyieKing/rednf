// ORIG 0x08426d18 sz 0xec（_ZN20DB_ReqQueryCharacInfo11makeRequestEiPKc，
// d_aa_body.cpp objdump 转写）：Acquire("DBThread.cpp",0x66eb)
//   → CStreamGuard(stream,true) → <<0xc4(int) → <<uid(int)
//   → GetInBuffer<SIG_QUERY_CHARAC_INFO> → strncpy(name,0x1d)
//   → put(DB_Q(2))。
void DB_ReqQueryCharacInfo::makeRequest(int uid, const char* name)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x66eb), true);
    guard.operator*()->operator<<((int)0xc4);
    guard.operator*()->operator<<((int)uid);
    SIG_QUERY_CHARAC_INFO* sig = guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    strncpy(sig->m_pad, name, 0x1d);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}