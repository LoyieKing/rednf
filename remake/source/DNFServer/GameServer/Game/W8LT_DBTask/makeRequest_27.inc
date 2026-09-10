void DB_ReqQueryCharacInfo::makeRequest(int uid, const char* name)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x66eb), true);
    guard.operator*()->operator<<((int)0xc4);
    guard.operator*()->operator<<((int)uid);
    SIG_QUERY_CHARAC_INFO* sig = guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    strncpy(sig->m_pad, name, 0x1d);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
