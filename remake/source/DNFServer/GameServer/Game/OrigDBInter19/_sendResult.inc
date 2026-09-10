void DB_SecurityCardIssue::_sendResult(int param_1, char const* param_2,
                                       char const* param_3, char const* param_4,
                                       char const* param_5, ENUM_ERROR error)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6dec), true);

    guard.operator*()->operator<<(0xfd);
    guard.operator*()->operator<<(param_1);

    SecuIssueView* view = reinterpret_cast<SecuIssueView*>(
        guard->GetInBuffer<SIG_SECURITY_CARD_ISSUE>());
    view->m_error = error;
    if (param_3 != 0)
    {
        strncpy(view->m_certKey, param_3, 0x21);
    }
    if (param_4 != 0)
    {
        strncpy(view->m_serverKey, param_4, 0x27);
    }
    if (param_5 != 0)
    {
        strncpy(view->m_phone, param_5, 5);
    }
    if (param_2 != 0)
    {
        strncpy(view->m_serial, param_2, 0xc);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
