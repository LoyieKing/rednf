// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 737（ORIG 892 / ours 893），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [BigDispatchCore] 实测 ndiff 483→466；两处 REAL 修正：
//  ① **REAL-BUG**：`std::string(notify.m_msg, sizeof(notify.m_msg))` 会把缓冲区尾部
//     未初始化字节纳入字符串（ORIG 0x081f3b63 只传 (const char*, alloc)，按 NUL 截断），
//     使下游 filtered.size()/strncpy 长度与上送 total 全部失真；
//  ② msgType==0x22 里 ORIG 0x081f4249(find_item 判空) 与 0x081f427f(is_stackable) 是
//     两个独立判定、各发一次 SendCmdErrorPacket(0x11,0x11)（合并成 || 少一次调用）。
// 残差封 N：std::string/allocator 的 C1↔C2、D1↔D2 变体（三方库，协议豁免）。
int DisPatcher_SendMess::dispatch_sig(CUser* user, PacketBuf& packet)

{
    if (user->get_state() < 3)
        return 0;

    char msgType;                                        // ORIG msg+0xd（-0x196）
    unsigned short target;                               // ORIG msg+0xe（-0x195）
    unsigned int recvCharno;                             // ORIG msg+0x10（-0x193）
    int msgLen;                                          // ORIG msg+0x36（-0x16d）
    char msgBuf[0x100];                                  // ORIG acStack_16d
    if (!packet.get_byte(*(char*)&msgType))
        return LineFunc(0x7ef5, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(target))
        return LineFunc(0x7ef6, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(recvCharno))
        return LineFunc(0x7ef7, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(msgLen))
        return LineFunc(0x7ef8, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(msgBuf, 0x100, msgLen))
        return LineFunc(0x7ef9, __PRETTY_FUNCTION__, 0, 0);

    G_CDataManager()->rewriteUsableEmoticon(*user, msgBuf);

    const bool isGhost = (bool)(unsigned char)user->IsCurCharacGhost();
    bool bypassGate = !isGhost ||
                      ((unsigned char)msgType < 9 &&
                       (((1u << msgType) & (unsigned long)0x144u) != 0));
    if (!bypassGate)
        return 0;

    int srcLen;                                          // ORIG -0x189/-0x193 别名区（昵称长）
    char nickBuf[0x1e];                                  // ORIG acStack_18f
    unsigned char channelFlag = 0;                       // ORIG local_62（-0x5e）
    char nickFlag = 0;                                   // ORIG local_6d（msg+0x13a 尾字节）

    if (msgType == 0x24 || msgType == 0x25 || msgType == 0x26)
    {
        // ---- 社区频道监视包（Community server） ----
        if (!packet.get_int(srcLen))
            return LineFunc(0x7f4b, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_str(nickBuf, 0x1e, srcLen))
            return LineFunc(0x7f4d, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_byte(*(char*)&nickFlag))
            return LineFunc(0x7f4f, __PRETTY_FUNCTION__, 0, 0);

        Packet_Monitor_Other_Channel_Chat notify;
        if (msgType == 0x24)
            notify.m_chatType = 7;
        else if (msgType == 0x25)
            notify.m_chatType = 0x21;
        else
            notify.m_chatType = 1;
        notify.m_id = user->get_acc_id();
        notify.server_id = nickFlag;
        notify.m_characNo = (unsigned int)user->getCurCharacNo();
        notify.m_recverCharId = (int)recvCharno;
        notify.m_buddyNameLen = (char)((unsigned int)srcLen & 0xff);
        // [DPC_BitsB 负结论 2026-09-11] disp2/bits 报本件 OURS-only `and $0xff`×2；
        // ORIG 0x081f3902/0x081f392a 该实参为 `mov -0x18f(%ebp),%eax; mov %eax,0x8(%esp)`
        // （-0x18f = ORIG 0x081f3774 get_int 的 srcLen 输出槽，无掩码）。
        // 试 `static_cast<unsigned char>(srcLen)` ⇒ 我方产 `mov; movzbl %al,%eax; mov`
        // （3 条，仍 ≠ ORIG 2 条），件级 ndiff 131 不变、四数不变 ⇒ 无收益已回滚。
        // 去掩码改 `srcLen` 可逐条对齐但会去掉 srcLen 的长度钳制（ORIG 无钳制 ⇒ 还原
        // 栈溢出面），按安全纪律不落。ORIG 地址：0x081f38fc / 0x081f3924。
        memcpy(notify.buddy_n_user_id_what, nickBuf,
               (size_t)srcLen & 0xffu);
        memcpy(notify.m_msg, msgBuf, (unsigned char)msgLen);
        notify.m_msgLen = (unsigned char)msgLen;

        GlobalData::s_community_proxy->SendTcpPacket(
            reinterpret_cast<char*>(&notify),
            (int)(unsigned short)(notify.m_msgLen + 0x3b));
        return 0;
    }

    if (msgType == 7 || msgType == 1 || msgType == 0x21)
    {
        // ---- 本地频道监视包（monitor server）：type==7 才真正上送 ----
        if (!packet.get_int(srcLen))
            return LineFunc(0x7f71, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_str(nickBuf, 0x1e, srcLen))
            return LineFunc(0x7f73, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_byte(channelFlag))
            return LineFunc(0x7f78, __PRETTY_FUNCTION__, 0, 0);

        if (msgType == 7)
        {
            Packet_Monitor_Other_Channel_Chat notify;
            notify.m_characNo = (unsigned int)user->getCurCharacNo();
            notify.m_buddyNameLen = (char)((unsigned int)srcLen & 0xff);
            // [DPC_BitsB 负结论] 同第一处（ORIG 0x081f3b0a/0x081f3b16 直传 srcLen 槽）；
            // `static_cast<unsigned char>` 形态实测 ndiff 不变 ⇒ 已回滚。
            memcpy(notify.buddy_n_user_id_what, nickBuf,
                   (size_t)srcLen & 0xffu);
            memcpy(notify.m_msg, msgBuf, (unsigned char)msgLen);
            notify.m_msgLen = (unsigned char)msgLen;

            // ORIG 0x081f3b63 call _ZNSsC1EPKcRKSaIcE —— 只有 (const char*, alloc) 两参，
            // 即按 NUL 截断构造。原先多传 sizeof(m_msg) 会把缓冲区尾部未初始化字节
            // 一并纳入字符串（REAL-BUG：filtered.size() 随之失真）。
            std::string filtered(CSyncSlangFilter::GetInstance()->Filter(
                std::string(notify.m_msg)));
            if (filtered.size() != 0)
            {
                strncpy(notify.m_msg, filtered.c_str(), filtered.size());
                notify.m_msgLen = (unsigned char)(msgLen + 0);   // 长度保持原值口径
                const int total =
                    (int)(unsigned short)(notify.m_msgLen + 0x3b);
                CMonitorServerProxy* proxy =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                        user->GetServerGroup());
                proxy->SendPacket(reinterpret_cast<char*>(&notify), total);
            }
            return 0;
        }
    }
    else if (msgType == 0xb || msgType == 0xc || msgType == 0x23)
    {
        // ---- 聊天消耗品 A：itemIdx∈{0x24,0x382,0x28d150,0x292090dd,0x29209138} ----
        const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target), (int)recvCharno);
        if (item == 0 ||
            (item->m_addInfo != 0x24 && item->m_addInfo != 0x382 &&
             item->m_addInfo != 0x28d150 && item->m_addInfo != 0x292090dd &&
             item->m_addInfo != 0x29209138))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
        int secu = (int)(unsigned char)
            GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x24);
        if (secu != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, (unsigned char)secu);
            return 0;
        }
        if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target),
                                (int)recvCharno))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0xd5);
            return 0;
        }
        if (!user->use_stackable((short)(unsigned short)recvCharno,
                                 (int)target, -1, -1))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
    }
    else if (msgType == 0xd || msgType == 0xf)
    {
        // ---- 聊天消耗品 B：itemIdx==0x25(d)/0x28992f(f) ----
        const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target), (int)recvCharno);
        if (item == 0)
            return LineFunc(0x8020, __PRETTY_FUNCTION__, 0, 0);
        if (!((item->m_addInfo == 0x25 && msgType == 0xd) ||
              (item->m_addInfo == 0x28992f && msgType == 0xf)))
            return LineFunc(0x8025, __PRETTY_FUNCTION__, 0, 0);
        int secu = (int)(unsigned char)
            GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x24);
        if (secu != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, (unsigned char)secu);
            return 0;
        }
        if (user->CheckItemLock(GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target),
                                (int)recvCharno))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0xd5);
            return 0;
        }
        if (!user->use_stackable((short)(unsigned short)recvCharno,
                                 (int)target, -1, -1))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
    }
    else if (msgType == 0xe)
    {
        // ---- 聊天消耗品 C：itemIdx==0x460 且 GM 才可用 ----
        const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target), (int)recvCharno);
        if (item == 0 || item->m_addInfo != 0x460)
            return LineFunc(0x8053, __PRETTY_FUNCTION__, 0, 0);
        if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
            return LineFunc(0x8057, __PRETTY_FUNCTION__, 0, 0);
        if (!user->use_stackable((short)(unsigned short)recvCharno,
                                 (int)target, -1, -1))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
    }
    else if (msgType == 0x22)
    {
        // ---- 聊天消耗品 D：itemIdx==0x1d9e（state==3 门禁，直接删） ----
        if (user->get_state() != 3)
            return 0;
        const Inven_Item* item = user->getCurCharacInvenR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target), (int)recvCharno);
        if (item == 0 || item->m_addInfo != 0x1d9e)
            return LineFunc(0x806a, __PRETTY_FUNCTION__, 0, 0);
        CItem* itemInfo = G_CDataManager()->find_item(item->m_addInfo);
        // ORIG 0x081f4249（find_item 判空）与 0x081f427f（is_stackable）是**两个**独立
        // 判定，各发一次 SendCmdErrorPacket(0x11,0x11) 后 return 0；合并成 || 会少一次调用。
        if (itemInfo == 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
        if (!itemInfo->is_stackable())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
        CInventory* invW = user->getCurCharacInvenW();
        if (!invW->delete_item((INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(int)target),
                               (int)recvCharno, 1, (eItemDelReason)3, true))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x11, 0x11);
            return 0;
        }
    }

    // ---- GM 存证 + 全服广播 ----
    if (GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()))
    {
        DB_InsertPoliceSaveChatting::makeRequest(
            (unsigned int)user->getCurCharacNo(), (char)msgType, msgBuf);
    }
    G_GameWorld()->send_chat_msg(user, (char)msgType, target, recvCharno,
                                 msgBuf, msgLen, nickBuf, srcLen,
                                 (ENUM_NOTIPACKET)0xc);
    return 0;
}
