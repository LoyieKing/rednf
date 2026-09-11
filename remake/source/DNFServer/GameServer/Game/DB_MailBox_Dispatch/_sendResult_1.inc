// ============================================================================
// 7. DB_ReqQueryCharacInfoMailbox::_sendResult —— ORIG 0x0843c224（_sendResult.md）：
//    StreamPool::Acquire("DBThread.cpp",0x8ed6) → CStreamGuard(stream,true) →
//    <<0x1f6 → <<slot → GetInBuffer<SIG_QUERY_CHARAC_INFO> → +0x20=errCode；
//    errCode==0 时 strncpy(name,0x1d)、+0x24=characNo、+0x28=(char)job、
//    +0x2a=(short)lev、+0x2c=(char)growType、+0x2d=(char)sex → put → 返 1。
// ============================================================================
bool DB_ReqQueryCharacInfoMailbox::_sendResult(int errCode, int slot, const char* name,
                                               int characNo, int job, int lev,
                                               int growType, int sex)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x8ed6), true);

    guard.operator*()->operator<<(0x1f6);
    guard.operator*()->operator<<(slot);

    SIG_QUERY_CHARAC_INFO* out = guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>();
    out->m_errCode = errCode;
    if (errCode == 0)
    {
        strncpy(out->m_name, name, 0x1d);
        out->m_characNo = characNo;
        out->m_job = (char)job;
        out->m_lev = (short)lev;
        out->m_growType = (char)growType;
        out->m_sex = (char)sex;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}