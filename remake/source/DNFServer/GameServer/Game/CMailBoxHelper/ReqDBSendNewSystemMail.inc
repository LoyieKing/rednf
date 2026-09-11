// [b28d-L8] N82 | 全帧槽号 -4 平移（lea -0x14 vs -0x18 等）跨版本槽基差；P248 三档不涉类型（ORIG 0x08556dxx）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：删 m 局部槽 + P136 bool 直存 +
// P211 enum 同型化（check_tu_routed 达标）。
int CMailBoxHelper::ReqDBSendNewSystemMail(const char* pSender, Inven_Item const& item,
                                           unsigned int nA, unsigned int nCharacNo,
                                           const char* pTitle, int nTitleLen,
                                           unsigned int nB,
                                           ENUM_SERVER_GROUP nServerGroup,
                                           bool bA, bool bB)
{
    // ORIG 0x085555e8
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x54f), true);
    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<(-1);
    SIG_MAILBOX_SEND_NEW_MAIL* msg =
        guard.operator->()->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    std::memset(msg, 0, 0x168);
    _makeSendMailData(msg, pSender, 0, item, nA, nCharacNo, pTitle, nTitleLen, nB, 0);
    // P0：ORIG 无 `m` 具名局部（msg 槽 -0xc 单一）；P136：bool 直存。
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_serverGroup = nServerGroup;
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_cashShopFlag = bA;  // +0x57
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_flag = bB;          // +0x167
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}