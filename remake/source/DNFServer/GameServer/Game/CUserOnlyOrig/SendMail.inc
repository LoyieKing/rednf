// ---------------------------------------------------------------------------
// CUser::SendMail(bool certified) T 0x08689f12 sz0x3ca
// 报告：docs/class_func_reports/CUser/SendMail.md
// 0x8e101 MSG_MAILBOX_SEND 载荷（m_pad8e101 块视图）：name@+0x11/nameLen@+0xd/
// money@+0x2e/itemId@+0x39；multimail flag@0x8e3ec 分流；疲劳礼物金
// limit=(long long)(money*(0.7+(float)(rand*0.01)))（0x8cf3988=0.01、
// 0x8cf3990=0.7 实测）；SIG_FIND_FACTORY_HUB 0x24 块入 DB_Q。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x08689f12：p167=CLEAR，gap=7；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08689f12：N82 封顶——帧/槽差 + setcc（p167 CLEAR，gap 7）。
void CUser::SendMail(bool certified)
{
    m_field8e3ed = true;                                   // ORIG movb $1,0x8e3ed
    if (m_multiMailFlag != 0) {
        SendMultiMail(certified);
        return;
    }
    // [H6 2026-09-11 单旋钮·否定分支在前] ORIG 0x08689f3a `movzbl certified; xor $0x1,%eax;
    // test %al,%al; je <主体>` ⇒ 原码是**非 certified 块在前**（失败块直落 + 主体落 je 目标）；
    // 旧 `if (certified) {主体} 尾部失败` 令 4.4 产 `cmpb $0; je <失败>`（块序相反）。
    // 改 `if (!certified) {失败} else {主体}`：主体缩进不变、指令块序与 ORIG 一致。
    if (!certified) {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x61, 0x87);
        memset(m_mailMsgData, 0, sizeof(m_mailMsgData));
    }
    else {
        MSG_CHECK_MAIL_RECEIVER check;
        check.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_0>(
            *reinterpret_cast<MSG_MAILBOX_SEND*>(m_mailMsgData));
        int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(this, reinterpret_cast<MSG_MAILBOX_SEND*>(m_mailMsgData)->m_name11, &check);
        // [Lane39 判据①/P222] ORIG 0x08689fcb `cmpl $0x0,-0x24(%ebp); je +0xfc` ⇒ je 落
        // **成功块**（失败块 SendCmdErrorPacket 直落 + 尾 jmp 跳过成功块）⇒ 源码极性为
        // `if (rc != 0) {失败} else {成功}`；旧 `if (rc == 0) {成功}` 反产 `jne 成功`
        // （多 1 jcc/1 jmp）。
        if (rc != 0) {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x61, static_cast<unsigned char>(rc));
        }
        else {
            bool fatigueGift = false;                      // local_21
            // [R21a08 ④ §27.4 读/求值次数] ORIG 0x0868a012..0x0868a029：find_item 实参处
            // 直读 +0x39（`mov 0x8e13a(%eax),%eax; mov %eax,%ebx` 跨 G_CDataManager 保活、
            // 无具名槽）；0x0868a074..0x0868a07f 再**独立重读一次**
            // （`mov 0x8e13a(%eax),%eax; test %eax,%eax`）⇒ 原码无 itemId 具名局部。
            // 旧形 `int itemId = ...` 只读 1 次（槽 -0x30）⇒ 两次读之间若字段被改写则分叉。
            CItem* item = G_CDataManager()->find_item(
                static_cast<int>(
                    reinterpret_cast<const MSG_MAILBOX_SEND*>(m_mailMsgData)->m_itemId39));
            if (item != 0) {
                // [H6 2026-09-11 单旋钮·短路条件复原] ORIG 0x08689f7x 段为 `call *%edx
                // (IsAvatarItem); xor $0x1,%eax; test %al,%al; je <false 岛>` + `call
                // get_rarity; cmp $0x1,%eax; jg <同一 false 岛>` ⇒ 原码是短路合条件
                // `!IsAvatarItem() && get_rarity() <= 1`；旧具名 `skip` 另占槽并产无 xor 形。
                if (!item->IsAvatarItem() && item->get_rarity() <= 1) {
                    fatigueGift = true;
                }
            }
            if (reinterpret_cast<const MSG_MAILBOX_SEND*>(m_mailMsgData)->m_itemId39 == 0) {
                fatigueGift = true;
            }
            IncrementTradeGold(reinterpret_cast<const MSG_MAILBOX_SEND*>(m_mailMsgData)->m_gold2e,
                               static_cast<int>(fatigueGift));
        }
        if (getCurCharacR() != 0) {
            unsigned short usedFatigue = getCurCharacUsedFatigue();
            int randv = get_rand_int(0x19);
            CInventory* inven = const_cast<CInventory*>(getCurCharacInvenR());
            int money = inven->get_money();
            long long limit = static_cast<long long>(
                static_cast<double>(money) * (0.7 + static_cast<float>(
                    static_cast<double>(randv) * 0.01)));
            if (static_cast<unsigned int>(limit) <=
                reinterpret_cast<const MSG_MAILBOX_SEND*>(m_mailMsgData)->m_gold2e) {
                // [A06 2026-09-10 P259] ORIG 0x08689f12 段同形。
                // [K7 2026-09-11 §6.5 常量盲区] ORIG 0x0868a16c `movl $0xa12b,0x8(%esp)`
                // = Acquire 的行号实参（前一条 0x0868a167 `mov 0x940bd6c,%eax`
                // = s_stream_pool）⇒ 原码行号 0xa12b；旧值 0x703d 使运行期锁/日志行号不符。
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xa12b), true);
                guard.operator*()->operator<<(0x18a);
                guard.operator*()->operator<<(static_cast<int>(GetUID()));
                SIG_FIND_FACTORY_HUB* sig =
                    guard->GetInBuffer<SIG_FIND_FACTORY_HUB>();
                memset(sig, 0, 0x24);
                sig->m_mId = get_acc_id();
                strncpy(sig->m_characName, reinterpret_cast<MSG_MAILBOX_SEND*>(m_mailMsgData)->m_name11,
                        static_cast<unsigned int>(reinterpret_cast<MSG_MAILBOX_SEND*>(m_mailMsgData)->m_nameLenD));
                sig->m_field21 = 0;
                int r = get_rand_int(0x14);
                if (r + 0x78 <= static_cast<int>(usedFatigue)) {
                    sig->m_field21 = 1;
                }
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
        }
        memset(m_mailMsgData, 0, sizeof(m_mailMsgData));                      // 清 MSG_MAILBOX_SEND 载荷
        return;
    }
}