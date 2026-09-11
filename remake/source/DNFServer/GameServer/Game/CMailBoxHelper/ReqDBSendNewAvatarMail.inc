// ---- 系统邮件 ----
void CMailBoxHelper::ReqDBSendNewAvatarMail(CUser* user, int nCharacNo, int nItemIdx,
                                            int nExpireDate, char cA, char cB,
                                            const char* pAgency, const char* pTitle,
                                            int nTitleLen)
{
    // ORIG 0x085561b0
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x7dd), true);
    guard.operator*()->operator<<(0xc5);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxSendNewMailAvatarV* m =
        reinterpret_cast<MailBoxSendNewMailAvatarV*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    m->m_type = 0;
    std::strncpy(m->m_senderName, user->getCurCharacName(), 0x1d);
    m->m_nCharacNo = nCharacNo;
    m->m_senderCharacNo = user->getCurCharacNo();
    m->m_avatar.set(nCharacNo, (unsigned int)nItemIdx, 0, nExpireDate, true, cA,
                    pAgency, 2, 0);
    m->m_titleLen = 0;
    std::memset(m->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        m->m_titleLen = (unsigned short)nTitleLen;
        std::strncpy(m->m_title, pTitle, m->m_titleLen);
    }
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    CItem* item = G_CDataManager()->find_item(nItemIdx);
    if (item != 0 && item->IsAvatarItem())   // ORIG vtable+0x10 虚调用
        ((CEquipItem*)item)->getAvatarSocket((int)cB, emblem);
    m->m_avatar.setJewelSocket(emblem);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}