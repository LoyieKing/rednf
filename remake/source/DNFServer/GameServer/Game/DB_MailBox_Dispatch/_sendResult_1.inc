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
