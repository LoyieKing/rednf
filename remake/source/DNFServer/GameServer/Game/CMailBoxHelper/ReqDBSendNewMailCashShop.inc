int CMailBoxHelper::ReqDBSendNewMailCashShop(CUser* user, Inven_Item const& item,
                                             unsigned int nDestCharacNo,
                                             unsigned int nAmount, bool bA,
                                             const char* pTitle, int nTitleLen)
{
    // ORIG 0x08556d5c
    if (G_GameWorld()->GetChannelType() == 7)
        return 0;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x95d), true);
    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_MAILBOX_SEND_NEW_MAIL* msg = guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    _makeSendMailData(msg, user->getCurCharacName(), user->getCurCharacNo(), item,
                      nDestCharacNo, nAmount, pTitle, nTitleLen, 0, 0);
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_serverGroup = (ENUM_SERVER_GROUP)0;
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_cashShopFlag = 1;  // movb $1,0x57
    if (!bA)
    {
        reinterpret_cast<CMailBoxView*>(user->GetMailBox())->m_streamList.push_back(guard.Get()); // +0x58
        return 0;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}