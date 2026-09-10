int CMailBoxHelper::_DoSendNewMail(CUser* user, Inven_Item const& item,
                                   unsigned int nDestCharacNo, unsigned int nAmount,
                                   bool bA, const char* pTitle, int nTitleLen,
                                   int nType)
{
    // ORIG 0x08555916
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x59b), false);
    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_MAILBOX_SEND_NEW_MAIL* msg =
        guard.operator->()->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    std::memset(msg, 0, 0x168);
    _makeSendMailData(msg, user->getCurCharacName(), user->getCurCharacNo(),
                      item, nDestCharacNo, nAmount, pTitle, nTitleLen, 0,
                      nType);
    // ORIG 0x08555a25 两步形：`call GetServerGroup; mov -0xc(%ebp),%edx;
    // mov %eax,0x15f(%edx)`（RHS-first，无 callee-saved 缓存）。
    msg->m_serverGroup = user->GetServerGroup();
    if (!bA)
    {
        reinterpret_cast<CMailBoxView*>(user->GetMailBox())->m_streamList.push_back(guard.Get()); // +0x58
        return 0;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
