int CMailBoxHelper::ReqDBSendNewSystemMultiMail(const char* pSender,
                                                Inven_Item const* itemBase,
                                                unsigned int nCount,
                                                unsigned int nA,
                                                unsigned int nCharacNo,
                                                const char* pTitle, int nTitleLen,
                                                unsigned int nB,
                                                ENUM_SERVER_GROUP nServerGroup,
                                                bool bA)
{
    // ORIG 0x08556b68
    if (nCount > 0xa || nCount == 0)
        return 0x11;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x917), true);
    guard.operator*()->operator<<(0x2a4);
    guard.operator*()->operator<<(-1);
    MailBoxSendSystemMultiMailV* msg =
        reinterpret_cast<MailBoxSendSystemMultiMailV*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL>());
    std::memset(msg, 0, 0x3a9);
    msg->m_unk122 = 0;
    std::strcpy(msg->m_senderName, pSender);
    std::vector<std::pair<int, int> >::const_iterator spare_iter;
    (void)spare_iter;
    for (unsigned int i = 0; i < nCount; ++i)
        msg->m_items[i].setCopy(itemBase[i]);
    msg->m_count = nCount;
    msg->m_nA = nA;
    msg->m_characNo = nCharacNo;
    std::strcpy(msg->m_title, pTitle);
    // ORIG store 序（+0x122 后）：m_count/+0x388(0x14=nCount) →
    // m_nA/+0x38c(0x18) → m_characNo/+0x11e(0x1c) → strcpy(0x20=pTitle) →
    // m_titleLen/+0x399(0x24) → m_nB/+0x390(0x28) →
    // m_serverGroup/+0x395(0x2c) → m_flag/+0x394(-0x2c 槽 movzbl 中转) →
    // m_unk3a5/+0x3a5($1)。
    // 残差（b27 判定）：m_titleLen 单条 N76 store 装载序（4.1 LHS 槽先 vs
    // 4.4 RHS 值先，N76 已异型边界内封顶），其余 0x390/0x395/0x394 序全等。
    msg->m_titleLen = nTitleLen;
    msg->m_nB = nB;
    msg->m_serverGroup = nServerGroup;
    msg->m_flag = bA;
    msg->m_unk3a5 = 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
