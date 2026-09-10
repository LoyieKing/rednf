bool CBossTower::InsertRewardItems()
{
    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081447d8..0x081447e4：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;

        for (int j = 0; j < m_memberReward[i].m_count; ++j)
        {
            // ORIG 0x081447f9 具名指针槽 -0x34 + P171 死指针判空（Ghidra C
            // `local_38 != 0 && *(int*)(...+0x2e) != -1`）。
            Inven_Item* reward = &m_memberReward[i].m_items[j];
            if (reward == 0 || reward->m_addInfo == -1)
                continue;

            int slotResult = -1;
            if (reward->m_addInfo == 0)
            {
                // 金币奖励：m_addInfo2 为金额（ORIG 三次 get_add_info 各用点重取，P48）
                const int gained = member->getCurCharacInvenW()->gain_money(
                    reward->get_add_info(), (eMoneyAddReason)5, true, 0);
                if (gained < reward->get_add_info())
                {
                    member->SendMoneyFullReason(
                        (ENUM_MONEY_FULL_REASON)0,
                        static_cast<unsigned long>(reward->get_add_info()),
                        static_cast<unsigned long>(gained));
                }
                slotResult = 0;
            }
            else
            {
                // ORIG else 分支重取 getCurCharacInvenW（与金币分支各一次，P48）
                slotResult = member->getCurCharacInvenW()->insertItemIntoInventory(
                    *reward, (eItemAddReason)0xb, true, true);
            }

            if (slotResult >= 0)
            {
                PacketGuard packet;
                packet.put_header(0, 0xe);
                packet.put_byte(reward->GetItemSpace());
                packet.put_short(1);
                member->getCurCharacInvenR()->MakeItemPacket(
                    (INVEN_TYPE)GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)reward->GetItemSpace()),
                    slotResult, packet);
                packet.finalize(true);
                member->Send(packet);
            }
            else
            {
                // 背包满 → 邮件补发
                // ORIG 0x08144a73..+0x2da：邮件分支入口两条 int 预清
                // （-0x28 写后未读死局部 P171 照抄；-0x24 expireSec P198③ 两段式）。
                int mailDeadLocal = 0;             // ebp-0x28（ORIG 只写不读）
                int expireSec = 0;                 // ebp-0x24
                char uniqueNo[0x18] = {0};         // ebp-0x64（ORIG .rodata 清零模板）

                CItem* itemData =
                    G_CDataManager()->find_item(reward->m_addInfo);
                if (itemData == 0)
                    continue;

                char title[256];                   // ebp-0x182 起 0x100 清零
                memset(title, 0, sizeof(title));
                char senderName[30];               // ebp-0x82 起 0x1e 清零
                memset(senderName, 0, sizeof(senderName));

                strncpy(senderName,
                        g_scriptStringManager_.findString(
                            4, "chn_game_server_msg_21", 0),
                        0x1d);
                strncpy(title,
                        g_scriptStringManager_.findString(
                            4, "chn_game_server_msg_22", 0),
                        0xff);

                // ORIG 0x08144b82 第一次 IsAvatarItem：仅决定是否换算到期秒
                // （P48：同一 getter 在两臂各出现 ⇒ 源码两次独立调用，勿缓存）。
                if (itemData->IsAvatarItem())
                {
                    // 有效期换算到期秒（0 或 0x16d=365 视为无限）
                    const int period = itemData->getUsablePeriod();
                    // ORIG 0x08144ba6..+0x42f：两个 je 同落 0 块（+0x431），
                    // 计算块 fall-through ⇒ 正向包裹（计算入 then、0 归 else 尾岛）。
                    if (period != 0 && period != 0x16d)
                        expireSec =
                            GlobalData::s_systemTime_.getCurSec() +
                            period * 0x15180;      // 天 → 秒
                    else
                        expireSec = 0;             // ORIG 0x08144bd1 movl $0,-0x24
                }

                // ORIG 0x08144bd8 第二次 IsAvatarItem：决定邮件类型。
                if (itemData->IsAvatarItem())
                {
                    // [b28d-solo-CBossTower] P0/P183：Ghidra C 中 genIPGNo 结果
                    // （pcVar8）与 get_endurance 结果（cVar2）均为寄存器伪变量、
                    // 非具名局部槽 ⇒ 内联进实参（右起求值序 genIPGNo 先于
                    // get_endurance，与 ORIG 一致）。
                    WongWork::CMailBoxHelper::ReqDBSendNewAvatarMail(
                        member, member->getCurCharacNo(), reward->m_addInfo,
                        expireSec,
                        static_cast<char>(
                            static_cast<CEquipItem*>(itemData)->get_endurance()),
                        0,
                        WongWork::CGenUniqueNo::genIPGNo(
                            WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE_3,
                            static_cast<unsigned int>(member->getCurCharacNo()),
                            uniqueNo),
                        title, static_cast<int>(strlen(title)));
                }
                else
                {
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                        senderName, *reward, 0,
                        static_cast<unsigned int>(member->getCurCharacNo()),
                        title, static_cast<int>(strlen(title)), 0xf,
                        member->GetServerGroup(), false, false);
                }
            }
        }
    }
    return 1;
