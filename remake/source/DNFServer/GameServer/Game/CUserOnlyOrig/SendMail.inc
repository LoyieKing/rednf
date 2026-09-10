void CUser::SendMail(bool certified)
{
    m_field8e3ed = true;                                   // ORIG movb $1,0x8e3ed
    if (m_pad8e101[0x2eb] != 0) {                          // +0x8e3ec multimail flag
        SendMultiMail(certified);
        return;
    }
    if (certified) {
        MSG_CHECK_MAIL_RECEIVER check;
        check.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_0>(
            *reinterpret_cast<MSG_MAILBOX_SEND*>(m_pad8e101));
        int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(this, m_pad8e101 + 0x11, &check);
        // [Lane39 判据①/P222] ORIG 0x08689fcb `cmpl $0x0,-0x24(%ebp); je +0xfc` ⇒ je 落
        // **成功块**（失败块 SendCmdErrorPacket 直落 + 尾 jmp 跳过成功块）⇒ 源码极性为
        // `if (rc != 0) {失败} else {成功}`；旧 `if (rc == 0) {成功}` 反产 `jne 成功`
        // （多 1 jcc/1 jmp）。
        if (rc != 0) {
            SendCmdErrorPacket((ENUM_CMDPACKET)0x61, static_cast<unsigned char>(rc));
        }
        else {
            bool fatigueGift = false;                      // local_21
            int itemId = static_cast<int>(
                reinterpret_cast<const MSG_MAILBOX_SEND*>(m_pad8e101)->m_itemId39);  // +0x39
            CItem* item = G_CDataManager()->find_item(itemId);
            if (item != 0) {
                bool skip;
                if (item->IsAvatarItem() || item->get_rarity() > 1) {
                    skip = false;
                }
                else {
                    skip = true;
                }
                if (skip) {
                    fatigueGift = true;
                }
            }
            if (itemId == 0) {
                fatigueGift = true;
            }
            IncrementTradeGold(reinterpret_cast<const MSG_MAILBOX_SEND*>(m_pad8e101)->m_gold2e,  // +0x2e
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
                *reinterpret_cast<unsigned int*>(m_pad8e101 + 0x2e)) {   // +0x8e12f
                // [A06 2026-09-10 P259] ORIG 0x08689f12 段同形。
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xa12b), true);
                guard.operator*()->operator<<(0x18a);
                guard.operator*()->operator<<(static_cast<int>(GetUID()));
                SIG_FIND_FACTORY_HUB* sig =
                    guard->GetInBuffer<SIG_FIND_FACTORY_HUB>();
                memset(sig, 0, 0x24);
                sig->m_mId = get_acc_id();
                strncpy(sig->m_characName, m_pad8e101 + 0x11,
                        *reinterpret_cast<unsigned int*>(m_pad8e101 + 0xd));  // +0x8e10e
                sig->m_field21 = 0;
                int r = get_rand_int(0x14);
                if (r + 0x78 <= static_cast<int>(usedFatigue)) {
                    sig->m_field21 = 1;
                }
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
        }
        memset(m_pad8e101, 0, 0x145);                      // 清 MSG_MAILBOX_SEND 载荷
        return;
    }
    SendCmdErrorPacket((ENUM_CMDPACKET)0x61, 0x87);
    memset(m_pad8e101, 0, 0x145);
