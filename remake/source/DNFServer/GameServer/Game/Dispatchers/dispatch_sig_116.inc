int Dispatcher_UnsealRandomOption::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08228e5e：随机选项解封（金币/卷轴双通道）。
    if (user == 0 || user->get_state() < 3 || user->getCurCharacR() == 0)
        return 0;
    if (user->CheckInTrade())
        return 0;

    short slotA = 0;                       // 目标装备槽位（ORIG get_short(short&)）
    unsigned short paySlot = 0;            // 支付槽位（0xffff=金币，ORIG get_short(unsigned short&)）
    if (!packet.get_short(slotA))
        return LineFunc(0xe67c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(paySlot))
        return LineFunc(0xe681,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if ((int)user->CheckItemLock(1, (int)slotA) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0xd5);
        return 0;
    }

    CInventory* invenW = user->getCurCharacInvenW();
    Inven_Item* item = invenW->GetInvenRef(1, (int)slotA);
    if (item == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }
    CItem* proto = G_CDataManager()->find_item(item->m_addInfo);
    if (proto == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }
    const bool unlockGateFail =
        !(proto->get_rarity() < 3 && item->GetUpgrade() < 0xb);
    if (unlockGateFail)
    {
        int secuErr = GlobalData::s_pSecuProtectionField->Check(
            user, (SECURITY_PROTCTION)0x25);
        if (secuErr != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191,
                                     (unsigned char)(secuErr & 0xff));
            return 0;
        }
    }

    if (!item->m_random.m_random.empty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x13);
        return 0;
    }

    int checkErr = _check_unseal_scroll_item(user, item, paySlot);
    if (checkErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191,
                                 (unsigned char)(checkErr & 0xff));
        return 0;
    }

    // key 打包：rarity@b0 | optionGrade@b1 | usableLevel@b2..3（ORIG 帧连续视图）。
    const int rarity = proto->get_rarity();
    const unsigned short usableLevel = (unsigned short)proto->getUsableLevel();
    const unsigned char optionGrade =
        (unsigned char)proto->GetRandomOptionGrade();
    const int key = rarity | (optionGrade << 8) | ((int)usableLevel << 16);
    std::map<int, int>& priceTable = G_CDataManager()->m_randomOption.m_map178;
    std::map<int, int>::const_iterator found = priceTable.find(key);
    if (found == priceTable.end())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x11);
        return 0;
    }

    if (paySlot == 0xffff)
    {
        const int goldCost = found->second;
        if (user->getCurCharacInvenR()->get_money() < goldCost)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 10);
            return 0;
        }
    }
    else
    {
        Inven_Item scroll = user->getCurCharacInvenR()->GetInvenSlot(1, paySlot);
        if (scroll.m_addInfo < 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x16);
            return 0;
        }
    }

    RandomOption* opts = &item->m_random.m_random;
    const bool gave = GlobalData::g_randomOptionHandle->give_option(
        (unsigned long)item->m_addInfo, (ENUM_RARITY)rarity,
        (int)usableLevel, proto->getItemGroupName(),
        (int)proto->GetRandomOptionGrade(), opts);
    if (!gave)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x13);
        return 0;
    }

    user->m_historyLog.RandomOption(*item, randomOptionReason::T_REGENERATION);
    if (paySlot == 0xffff)
    {
        statistc_proxy::add(user, "log_random_option", "unseal_gold",
                            (unsigned int)found->second, "%d,'%s'",
                            user->get_charac_10_level_section(),
                            GlobalData::s_systemTime_.getCurDate());
        if (user->getCurCharacInvenW()->use_money(found->second,
                                                  (eMoneySubReason)0x2c, true) != 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 10);
            return 0;
        }
    }
    else
    {
        statistc_proxy::add(user, "log_random_option", "unseal_cnt", 1u,
                            "%d,'%s'", user->get_charac_10_level_section(),
                            GlobalData::s_systemTime_.getCurDate());
        if (user->getCurCharacInvenW()->use_item((int)paySlot, 1, 1) != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x191, 0x16);
            return 0;
        }
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)paySlot);
    }

    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x191);
    PacketGuard packetOut;
    packetOut.put_header(0, 0xe);
    packetOut.put_byte(0);
    packetOut.put_short(1);   // ORIG put_short(1)（NEW 缺此字段，回包格式差异）
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, (int)slotA,
                                               packetOut);
    packetOut.finalize(true);
    user->Send(packetOut);
    return 0;
