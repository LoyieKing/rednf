int DB_ReqQueryCharacInfo::_sendResult(int errCode, int slot, char const* name,
                                       int characNo, int job, int lev,
                                       int growType, int sex, int field2e)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6716), true);

    guard.operator*()->operator<<(0xc4);
    guard.operator*()->operator<<(slot);

    QueryCharacInfoView* view = reinterpret_cast<QueryCharacInfoView*>(
        guard->GetInBuffer<SIG_QUERY_CHARAC_INFO>());
    view->m_errCode = errCode;
    if (errCode == 0)
    {
        strncpy(view->m_name, name, 0x1d);
        view->m_characNo = characNo;
        view->m_job = static_cast<unsigned char>(job);
        view->m_lev = static_cast<short>(lev);
        view->m_growType = static_cast<unsigned char>(growType);
        view->m_sex = static_cast<unsigned char>(sex);
        view->m_field2e = static_cast<unsigned char>(field2e);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
