int Inter_LoadCharacLinkBonus::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x84e1f0e/0x84e1f15 两 int 前置清零（`movl $0x0,-0x28/-0x24`）后再由
    // msg 字段赋值（084e2068 起）⇒ 声明=0 + 独立赋值语句（P197③），pPkt 槽在 Inven_Item
    // 构造之后才物化（084e205f）。
    unsigned int bonusExp = 0;                         // -0x28（msg+0x0）
    int bonusGold = 0;                                 // -0x24（msg+0x4）

    char title[0x15] = { '\0' };                       // -0x3d（gcc 内联清零展开）
    char body[0x100] = { '\0' };                       // -0x1a5

    strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_149", 0),
            0x14);
    strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_150", 0),
            0xff);

    Inven_Item item;                                   // -0xa5（默认构造）

    Inter_LoadCharacLinkBonusPktView* pPkt = reinterpret_cast<Inter_LoadCharacLinkBonusPktView*>(data);
    bonusExp = pPkt->m_0;
    bonusGold = pPkt->m_4;

    if (bonusExp != 0 || bonusGold != 0)
    {
        if (bonusGold != 0)
        {
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, (unsigned int)bonusGold,
                (unsigned int)user->getCurCharacNo(), body,
                (int)strlen(body), 0,
                (ENUM_SERVER_GROUP)user->GetServerGroup(), false, false);

            char name[0x1e] = { '\0' };                // -0x5b
            const int slotIdx = (int)user->getLinkCharacSlotIndex();   // -0x1c（movsbl）
            if (slotIdx >= 0 &&
                (int)user->m_characList.size() > slotIdx)
            {
                strcpy(name, user->m_characList[slotIdx].m_name);  // 元素 +0x04
            }

            PacketGuard packet;                        // -0x68
            packet.clear();
            packet.put_header(0, 0x123);
            packet.put_int((int)strlen(name));
            packet.put_str(name, (int)strlen(name));
            packet.put_int(bonusGold);
            packet.put_int(0);
            packet.put_short(0);
            packet.put_short(0);
            packet.finalize(true);
            user->Send(packet);
            GetInstanceCirculationStatistic()->AddCirculationStatistic(
                (CIRCULATION_STATISTIC_FIELD)0x11, user,
                (unsigned int)bonusGold);
        }

        if (bonusExp != 0 && user->get_charac_level() != 0x46)
        {
            user->giveCharacLinkBonusExp(bonusExp);
        }

        user->UpdateCharacLinkBonus(0, 0,
                                    (unsigned int)user->getCurCharacNo(),
                                    (ENUM_CHARAC_LINK_BONUS_STATE)1);
        user->setCharacLinkBonusSetting(
            user->get_charac_idx((unsigned int)user->getCurCharacNo()), false);
    }
    return 0;
