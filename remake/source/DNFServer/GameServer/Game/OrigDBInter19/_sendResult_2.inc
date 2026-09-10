void DB_SecurityCardRetransfer::_sendResult(int param_1, char* param_2,
                                            char const* param_3,
                                            char const* param_4, ENUM_ERROR error)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6fd2), true);

    guard.operator*()->operator<<(0x138);
    guard.operator*()->operator<<(param_1);

    SecuRetransferView* view = reinterpret_cast<SecuRetransferView*>(
        guard->GetInBuffer<SIG_SECURITY_CARD_RETRANSFER>());
    view->m_error = error;
    if (param_3 != 0)
    {
        strncpy(view->m_certKey, param_3, 0x21);
    }
    if (param_4 != 0)
    {
        strncpy(view->m_serverKey, param_4, 0x27);
    }
    if (param_2 != 0)
    {
        strncpy(view->m_serial, param_2, 0xc);
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
