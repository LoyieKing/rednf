int CMailBoxHelper::ReqDBCheckReceiver_forchina(CUser* user, const char* name,
                                                MSG_CHECK_MAIL_RECEIVER* msg)
{
    // ORIG 0x08555486：同名拒绝(7)；否则 DB_Q 0x7e 校验请求
    if (std::strcmp(user->getCurCharacName(), name) == 0)
        return 7;

    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("MailBox.cpp", 0x52e),
                       true);
    guard.operator*()->operator<<(0x7e);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_MAILBOX_CHECK_RECEIVER* p = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    std::strncpy(p->m_receiverName, name, 0x1d);
    std::memcpy(p->m_payload, msg, 0x3f88);
    p->m_senderCharacNo = (unsigned int)user->get_charac_no(-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}