int Dispatcher_ItemHyperLinkMessage::process(CUser* user, MSG_BASE& msg,
                                             ParamBase& param)
{
    // §6.2 语义完成：P167 调用点差集双零（60/60）；orig583/our583，gap0，
    // 残余为寄存器/槽分配文本差（N82），非缺语义 → byte 层 N 封顶。
    stItemHyperLinkMessage& outParam =
        static_cast<stItemHyperLinkMessage&>(param);
    MSG_ITEM_HYPER_LINK_MESSAGE& m =
        reinterpret_cast<MSG_ITEM_HYPER_LINK_MESSAGE&>(msg);

    int err = check_error(user, msg);
    outParam.m_4 = err;
    if (err >= 1)
        return 0;
    if (err < 0)
    {
        return LineFunc(0x47aa,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }

    G_CDataManager()->rewriteUsableEmoticon(*user, m.m_text18);

    const bool ghostGateFail =
        user->IsCurCharacGhost() &&
        !(m.m_typeD < 9 && ((1u << (m.m_typeD & 0x1f)) & 0x144U) != 0);
    if (ghostGateFail)
        return 0;

    if (m.m_typeD == 0x26)
    {
        // 跨服频道转发（Packet_Monitor_Other_Channel_Chat_Hyper_Link，
        // ORIG 0x081d9eaf 区；ctor W 0x0822c3e6）。
        Packet_Monitor_Other_Channel_Chat_Hyper_Link pkt;
        pkt.m_chatType = 1;
        pkt.m_id = user->get_acc_id();
        pkt.m_characNo = user->getCurCharacNo();
        pkt.server_id = m.m_field13a;
        pkt.m_recverCharId = m.m_slot10;
        pkt.m_buddyNameLen = (char)m.m_nameLen118;
        pkt.m_msgLen = (unsigned char)m.m_textLen14;
        memcpy(pkt.buddy_n_user_id_what, m.m_name11c, m.m_nameLen118);
        memcpy(pkt.m_msg, m.m_text18, m.m_textLen14);
        pkt.m_itemCount = m.m_linkCount13b;
        for (int i = 0; i < m.m_linkCount13b; i++)
            memcpy(pkt.m_items[i] + 0xb, m.m_links13c[i] + 0xc, 0x68);
        pkt.packetSize = (unsigned short)(0x174 + m.m_textLen14);
        GlobalData::s_community_proxy->SendTcpPacket(
            reinterpret_cast<char*>(&pkt), pkt.packetSize);
        return 0;
    }

    unsigned int itemSpace = m.m_itemSpaceE;
    int slot = (int)m.m_slot10;
    if (m.m_typeD == 0xb || m.m_typeD == 0xc || m.m_typeD == 0x23)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        const Inven_Item* ref =
            invenR->GetInvenRef(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned int)itemSpace), slot);
        const unsigned int addInfo = *(const unsigned int*)&ref->m_addInfo;
        if (ref == 0 ||
            !(*(const int*)&ref->m_addInfo == 0x24 ||
              *(const int*)&ref->m_addInfo == 0x382 ||
              *(const int*)&ref->m_addInfo == 0x28d150 ||
              *(const int*)&ref->m_addInfo == 0x292090dd ||
              *(const int*)&ref->m_addInfo == 0x29209138))
        {
            (void)addInfo;
            return LineFunc(0x4801,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
        int secuErr = GlobalData::s_pSecuProtectionField->Check(
            user, (SECURITY_PROTCTION)0x24);
        if (secuErr != 0)
        {
            outParam.m_4 = secuErr;
            return 0;
        }
        if (user->CheckItemLock(
                (int)GetInvenTypeFromItemSpace(
                    (ENUM_ITEMSPACE)(unsigned int)itemSpace),
                slot) != 0)
        {
            outParam.m_4 = 0xd5;
            return 0;
        }
        if (!user->use_stackable((short)slot, (int)itemSpace, -1, -1))
        {
            outParam.m_4 = 0x11;
            return 0;
        }
    }
    else if (m.m_typeD == 0xd || m.m_typeD == 0xf)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        const Inven_Item* ref =
            invenR->GetInvenRef(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned int)itemSpace), slot);
        if (ref == 0)
            return LineFunc(0x4821,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        const int idxVal = *(const int*)&ref->m_addInfo;
        if (!((idxVal == 0x25 && m.m_typeD == 0xd) ||
              (idxVal == 0x28992f && m.m_typeD == 0xf)))
            return LineFunc(0x4826,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        int secuErr = GlobalData::s_pSecuProtectionField->Check(
            user, (SECURITY_PROTCTION)0x24);
        if (secuErr != 0)
        {
            outParam.m_4 = secuErr;
            return 0;
        }
        if (user->CheckItemLock(
                (int)GetInvenTypeFromItemSpace(
                    (ENUM_ITEMSPACE)(unsigned int)itemSpace),
                slot) != 0)
        {
            outParam.m_4 = 0xd5;
            return 0;
        }
        if (!user->use_stackable((short)slot, (int)itemSpace, -1, -1))
        {
            outParam.m_4 = 0x11;
            return 0;
        }
    }
    else if (m.m_typeD == 0xe)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        const Inven_Item* ref =
            invenR->GetInvenRef(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned int)itemSpace), slot);
        if (ref == 0 || *(const int*)&ref->m_addInfo != 0x460)
            return LineFunc(0x4847,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
            return LineFunc(0x484b,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        if (!user->use_stackable((short)slot, (int)itemSpace, -1, -1))
        {
            outParam.m_4 = 0x11;
            return 0;
        }
    }
    else if (m.m_typeD == 0x22)
    {
        if (user->get_state() != 3)
            return 0;
        const CInventory* invenR = user->getCurCharacInvenR();
        const Inven_Item* ref =
            invenR->GetInvenRef(
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned int)itemSpace), slot);
        if (ref == 0 || *(const int*)&ref->m_addInfo != 0x1d9e)
            return LineFunc(0x485d,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        CItem* proto = G_CDataManager()->find_item(*(const int*)&ref->m_addInfo);
        if (proto == 0 || !proto->is_stackable())
        {
            outParam.m_4 = 0x11;
            return 0;
        }
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)itemSpace), slot, 1,
                (eItemDelReason)3, true))
        {
            outParam.m_4 = 0x11;
            return 0;
        }
    }

    if (GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
    {
        DB_InsertPoliceSaveChatting::makeRequest(
            user->getCurCharacNo(), (char)m.m_typeD, m.m_text18);
    }
    G_GameWorld()->send_chat_msg_hyper_link(user, reinterpret_cast<MSG_ITEM_HYPER_LINK_MESSAGE&>(m),
                                            (ENUM_NOTIPACKET)0x172);
    return 0;
