int Inter_NpcLimitBuyItemInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    reset();                                           // ORIG 0x084e6e1a（已实现）

    // ORIG 单指针局部（-0xc）：包视图与 checkError 实参共用同一槽；声明在 reset() 之后
    // （084e65b8 才 `mov %eax,-0xc(%ebp)`）。
    Inter_NpcLimitBuyItemInfoPktView* msg =
        reinterpret_cast<Inter_NpcLimitBuyItemInfoPktView*>(data);
    LimitNpcBuyItemUpdate update;                      // -0x2e（0x1a 包）

    CUser* target =                                    // +0xc
        G_CGameManager()->getUser(
            msg->m_itemIdReq, 10);

    if (target == 0)
    {   // 用户不在线：按本类缓存字段回 monitor（ORIG GetServerGroup(NULL) 原样保留）
        update.m_errorNo     = 3;             // +0x16（084e65f1 movl $0x3,-0x18）
        update.m_itemId      = msg->m_itemId; // +0x0a（084e6604）
        update.m_charNo      = msg->m_charNo; // +0x0e（084e6601）
        update.m_cancelCount = msg->m_count;  // +0x12（084e660a）
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&update), 0x1a);
        return 0;
    }

    // ORIG 084e6654/084e665e：0x22 按 32 位读（mov 0x22(%eax)），
    // test 判全字；实参仅取低字节（movzbl %al）传 SendCmdErrorPacket。
    if (msg->m_errorNo != 0)   // 请求自带错误码
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1d1,
                                 (unsigned char)msg->m_errorNo);
        return 0;
    }

    const int err = checkError(
        user, reinterpret_cast<LimitNpcBuyItemInfo*>(msg));   // -0x18（ORIG 返回 int）
    if (err != 0)
    {
        update.m_itemId      = m_error;                // +0x0a（084e66af this+0x4）
        update.m_charNo      = (unsigned int)user->get_charac_no(-1);  // +0x0e
        update.m_cancelCount = msg->m_count;           // +0x12（084e66ce）
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&update), 0x1a);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1d1, (unsigned char)err);
        return 0;
    }

    m_invenItem1.m_addInfo = m_error;                  // +0x1f = +0x04（item idx）
    G_CDataManager()->m_itemList->create_item(
        (ENUM_ITEM_CREATE_TYPE)0, m_invenItem1, 1);

    m_param1 = user->getCurCharacInvenW()->insertItemIntoInventory(   // +0x10
        m_invenItem1, (eItemAddReason)0, true, false);
    m_data1c = 0;                                      // +0x1c

    if (m_param1 < 0)
    {   // 入包失败：系统邮件兜底
        std::string strTitle =                         // -0x14
            g_ServerString_.GetServerString(0x436, 0);
        char title[0x14];                              // -0x43
        strncpy(title, strTitle.c_str(), 0x14);
        std::string strBody =                          // -0x10
            g_ServerString_.GetServerString(0x437, 0);
        char body[0xff];                               // -0x143
        strncpy(body, strBody.c_str(), 0xff);

        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, m_invenItem1, 0,
                (unsigned int)user->get_charac_no(-1), body,
                (int)strlen(body), 0x1e,
                user->GetServerGroup(), false, false) == 0)
        {
            // ORIG 084e6989..084e69d9：0x145 清零后写 -0x288+0x11(m_name11, strncpy 0x14)、
            // -0x288+0x39(m_itemId39=m_addInfo)、-0x288+0x3d(m_extra3d=m_addInfo2)
            // ⇒ 即权威 MSG_MAILBOX_SEND 具名成员（原 NpcLimitMailView 槽位 +0x01/+0x35 为误读）。
            MSG_MAILBOX_SEND mail;
            memset(&mail, 0, 0x145);
            strncpy(mail.m_name11, title, 0x14);
            mail.m_itemId39 = m_invenItem1.m_addInfo;
            mail.m_extra3d = m_invenItem1.m_addInfo2;
            user->m_historyLog.SendMail(               // ORIG (user+0x79700)
                &mail, 0);
        }
        m_data1c = 1;
    }

    user->getCurCharacInvenW()->delete_item(
        (INVEN_TYPE)1, m_param2, m_needItem.first,
        (eItemDelReason)0x1a, true);

    update.m_errorNo     = 0;                          // +0x16（084e6a48）
    update.m_itemId      = m_error;                    // +0x0a（084e6a52 this+0x4）
    update.m_charNo      = (unsigned int)user->get_charac_no(-1);  // +0x0e（084e6a6b）
    update.m_cancelCount = msg->m_count;               // +0x12（084e6a71）
    {
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&update), 0x1a);
    }
    sendPacket(user);
    return 0;
