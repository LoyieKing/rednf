void CMailBoxHelper::ReqDBSendNewCreatureMailCashShop(CUser* user, int nCharacNo,
                                                      Inven_Item const& item,
                                                      const char* pTitle, int nTitleLen)
{
    // ORIG 0x085571a0：同 CreatureMail，另 +0x1a2=1
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x99c), true);
    guard.operator*()->operator<<(0xc5);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxSendNewMailCreatureV* m =
        reinterpret_cast<MailBoxSendNewMailCreatureV*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    m->m_type = 1;
    std::strncpy(m->m_senderName, user->getCurCharacName(), 0x1d);
    m->m_nCharacNo = nCharacNo;
    m->m_senderCharacNo = user->getCurCharacNo();
    std::memset(&m->m_pad2c[0], 0, 0x74);
    m->m_item.setCopy(item);   // Inven_Item_Compatible::setCopy（ORIG 0x0855651e）
    reinterpret_cast<MailBoxCreatureSeedView*>(m)->m_seed = 0;  // movl $0,0x58
    m->m_nDestCharacNo = nCharacNo;
    m->m_unk70 = 100;
    m->m_unk7d = 0x20;
    m->m_unk7f = 1;
    m->m_unk94 = 0;
    m->m_cashShopFlag = 1;   // +0x1a2
    register int addInfo = item.m_addInfo;
    CItem* found = G_CDataManager()->find_item(addInfo);
    if (found == 0)
        return;
    if (found->is_stackable())
        return;
    CEquipItem* eq = (CEquipItem*)found;
    if (!eq->get_sub_type())
        m->m_subType0 = 1;
    else
        m->m_subType0 = 0;
    // ORIG 0x0855728x（CashShop 专用序）：usablePeriod/expiration 紧跟
    // m_subType0 段、先于 titleLen/memset/strncpy（与 CreatureMail 相反）。
    m->m_usablePeriod = eq->getUsablePeriod();
    m->m_expiration = eq->getExpirationDate();
    m->m_titleLen = 0;
    std::memset(m->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        m->m_titleLen = (unsigned short)nTitleLen;
        std::strncpy(m->m_title, pTitle, m->m_titleLen);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
