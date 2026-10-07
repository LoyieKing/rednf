// ORIG T 0x081d9c4a：超链接聊天处理（PacketDispatcher_Impl_1.cpp 0x47aa 区）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d9c4a orig/ours=583/583 ndiff-blocks=105
//   帧差 sub $2e0 vs $2f0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x2e0,%esp | mov    -0x54(%ebp),%edx | mov    %eax,0x4(%edx)
int Dispatcher_ItemHyperLinkMessage::process(CUser* user, MSG_BASE& msg,
                                             ParamBase& param)
{
    // §6.2 语义完成：P167 调用点差集双零（60/60）；orig583/our583，gap0，
    // 残余为寄存器/槽分配文本差（N82），非缺语义 → byte 层 N 封顶。
    stItemHyperLinkMessage& outParam =
        static_cast<stItemHyperLinkMessage&>(param);
    // [DispLaneB 2026-09-11] P0/P183：ORIG 本函数**无** msg 具名槽 —— 反汇编
    //   0x081d9c5b 起 `mov 0x10(%ebp),%eax` 形参直读 6 次，全函数唯一入口局部槽
    //   -0x54 属 outParam（`mov 0x14(%ebp),%eax; mov %eax,-0x54(%ebp)`）。我方原
    //   引用局部 m 多占 4B 槽并抬整帧 0x10（our 0x2f0 vs ORIG 0x2e0）。改用编译期
    //   内联式 → 每个 m.xxx 在使用点经形参直读（语义不变、零新增符号）。
#define m (reinterpret_cast<MSG_ITEM_HYPER_LINK_MESSAGE&>(msg))

    // [DispLaneB 2026-09-11 实测回滚] ORIG 侧虽无 err 槽（0x08202cff 结果直存
    //   outParam.m_4、判定从该成员重读），但删 err 后 4.4 生成 585 条/ndiff 94
    //   （原形 581 条/ndiff 83）⇒ 按 codegen 轴闸门保留具名 err（差 2 条 vs 少 11 块）。
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
        static_assert(sizeof(pkt.m_items[0]) == 0x68, "hyperlink item stride");
        for (int i = 0; i < m.m_linkCount13b; i++)
            // [Q4DispatchBig 2026-09-11 · 偏移轴真 bug 修复] 旧译 src 多 `+ 0xc`：
            //   ORIG 0x081d9e11..081d9e45 为 `imul $0x68,%eax; add $0x130,%eax;
            //   add msg,%eax; lea 0xc(%eax),%edx` ⇒ src 绝对 = msg + 0x13c + i*0x68
            //   （0x130+0xc = 成员基址 0x13c，**非**额外偏移）；ORIG 反编译直证
            //   `memcpy(dst+i*0x68, msg + i*0x68 + 0x13c, 0x68)`（报告 :689）。
            //   我方 `m_links13c[i] + 0xc` = msg + 0x148 + i*0x68 ⇒ 复制源每项错位
            //   12 字节（跨服物品链接显示错乱，玩家可见）。同成员全库使用点
            //   （GameWorld.cpp:4656/5268/5291/5314/5333 + 本文件 :17551）均无 `+0xc`。
            // [bughunt18] ORIG 0x081d9e11..0x081d9e45：dst = pkt + 0x30 + 0xb + i*0x68
            //   = &pkt.m_items[i]（m_items 已在 +0x3b）。旧 `+ 0xb` 让 dst 落到
            //   +0x46+i*0x68：每条链接错位 11B，第 3 条还覆盖 m_msgLen/m_msg 头。
            memcpy(pkt.m_items[i], m.m_links13c[i], 0x68);
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
    G_GameWorld()->send_chat_msg_hyper_link(user, m,
                                            (ENUM_NOTIPACKET)0x172);
    return 0;
}