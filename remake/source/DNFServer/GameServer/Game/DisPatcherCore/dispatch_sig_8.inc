int DisPatcher_SellItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char invenType;                     // -0x45
    short slot;                         // -0x48
    unsigned short count;               // -0x46
    unsigned long money;                // -0x64
    int sum;                            // -0x3c
    int unusedGuard;                    // -0x38（ORIG 死存储 movl $0x6）
    int sellResult;                     // -0x34
    int sellTimeRet;                    // -0x30
    unsigned int nowTick;               // GetCurTickCount 返回（局部）
    void* party;                        // -0x2c
    CMap* map;                          // -0x24
    Village* village;                   // -0x20
    Area* area;                         // -0x1c
    Inven_Item* invenItem;              // GetInvenItemPtr（寄存器传递）
    CItem* item;                        // GetItemPtr（寄存器传递）
    int rarity;                         // -0x40

    if (user->get_state() == 3 || user->get_state() == 8 ||
        user->get_state() == 5 || user->get_state() == 10 ||
        user->get_state() == 0xc)
        return 0;

    if (packet.get_byte(invenType) != 1)
        return LineFunc(0x7e15, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_short(slot) != 1)
        return LineFunc(0x7e16, __PRETTY_FUNCTION__, 0, 0);

    if (packet.get_short(count) != 1)
        return LineFunc(0x7e17, __PRETTY_FUNCTION__, 0, 0);

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 3))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0xd1);
        return 0;
    }

    // P48：GetInvenItemPtr / GetItemPtr 为两次独立调用（各自求值实参）
    invenItem = GlobalData::s_pSecuProtectionField->GetInvenItemPtr(
        user, invenType, slot);
    item = GlobalData::s_pSecuProtectionField->GetItemPtr(user, invenType, slot);
    rarity = GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x23, item, invenItem);
    if (rarity != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, (unsigned char)rarity);
        return 0;
    }

    if (user->CheckItemLockSlotSpace(invenType, slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0xd5);
        return 0;
    }

    if (user->get_state() == 5 &&
        user->getInDungeonItemSellCount(invenType, slot) == 7)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x139,
                                       1, 0, 0);
    }

    PacketGuard packetGuard;            // -0x60
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        packetGuard.clear();
        packetGuard.put_header(1, 0x18);
        packetGuard.put_byte(0);
        packetGuard.put_byte(0x3c);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        return 0;
    }

    money = 0;
    if (packet.get_int(money) != 1)
        return LineFunc(0x7e71, __PRETTY_FUNCTION__, 0, 0);

    sum = 0;
    if ((int)(signed char)invenType + (int)slot + (unsigned short)count + sum !=
        (int)money)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x335,
                                       1, 0, 0);
    }

    unusedGuard = 6;                    // ORIG -0x38 死存储
    if (invenType == 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0x17);
        return 0;
    }
    if (invenType == 3 && slot <= 9)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18, 0x17);
        return 0;
    }

    if (user->get_state() == 5)
    {
        party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            // CParty+0xb24 内嵌 CBattle_Field（局部视图 CPartyBattleFieldView）
            CBattle_Field* battleField =
                &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField;
            map = (CMap*)G_CDataManager()->find_map(battleField->get_cur_map());
            if (map != 0 && map->getAssignedNPCCount() == 0 &&
                !battleField->IsAssignNPCByPotion())
            {
                WongWork::CHackAnalyzer* hackAnalyzer = user->getHackAnalyzer();
                hackAnalyzer->addServerHackCnt(user,
                                               (WongWork::ENUM_HACKTYPE)0x323,
                                               1, 0, 0);
            }
        }
    }

    if (user->get_state() == 3)
    {
        if (G_GameWorld()->check_valid_area((int)user->getCurCharacVill(),
                                            user->get_area(false)))
        {
            village = G_GameWorld()->getUserVillage(user);
            if (village != 0)
            {
                area = village->getArea(user->get_area(false));
                if (area != 0 && !area->IsExistItemShopNPC())
                {
                    user->getHackAnalyzer()->addServerHackCnt(
                        user, (WongWork::ENUM_HACKTYPE)0x324, 1, 0, 0);
                }
            }
        }
    }

    sellResult = G_Store()->user_sell_item(user, invenType, slot, count);
    if (sellResult != 0)
    {
        packetGuard.put_header(1, 0x18);
        packetGuard.put_byte(0);
        packetGuard.put_byte(sellResult);
        packetGuard.finalize(true);
        user->Send(packetGuard);
        if (sellResult == 0x16)
            user->SendMoneyFullReason((ENUM_MONEY_FULL_REASON)3, 0, 0);
    }

    sellTimeRet = 0;
    nowTick = GlobalData::s_systemTime_.getCurTickCount();
    sellTimeRet = user->SellItemTimeChecker(nowTick, 1000);
    if (sellTimeRet == 2)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x138,
                                       1, 0, 0);
    }
    else if (sellTimeRet == 4)
    {
        user->getHackAnalyzer()->addServerHackCnt(user, (WongWork::ENUM_HACKTYPE)0x321,
                                       1, 0, 0);
    }

    return 0;
