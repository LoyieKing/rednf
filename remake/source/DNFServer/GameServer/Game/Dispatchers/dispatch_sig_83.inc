int Dispatcher_UpgradeChronicle::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08223330：三代编年史强化（WongWork::CItemUpgrade::ProcUpgradeChronicle）。
    if (user->get_state() < 3)
        return 0;

    short slotA = 0;
    if (!packet.get_short(slotA))
        return LineFunc(0xdda4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned int seqA = 0;
    if (!packet.get_int(seqA))
        return LineFunc(0xdda5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    short slotB = 0;
    if (!packet.get_short(slotB))
        return LineFunc(0xdda6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    unsigned int seqB = 0;
    if (!packet.get_int(seqB))
        return LineFunc(0xdda7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    char chanceGrade = 0;
    if (!packet.get_byte(chanceGrade))
        return LineFunc(0xdda8,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // ORIG 0x08223484：CSecu_ProtectionField::Check(user, 0x1a)，
    // 非 0 则 SendCmdErrorPacket(0x112, 错误码) 返回 0。
    int secuErr = GlobalData::s_pSecuProtectionField->Check(
        user, (SECURITY_PROTCTION)0x1a);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, (unsigned char)secuErr);
        return 0;
    }

    if ((int)user->CheckItemLock(1, (int)slotB) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0xd5);
        return 0;
    }

    CInventory* inven = user->getCurCharacInvenW();
    Inven_Item itemA = inven->GetInvenSlot(1, (int)slotA);
    Inven_Item itemB = inven->GetInvenSlot(1, (int)slotB);

    CStackableItem* chronicle =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(itemA.m_addInfo));
    CItem* equipItem = G_CDataManager()->find_item(itemB.m_addInfo);
    if (chronicle == 0 || equipItem == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 1);
        return 0;
    }
    if (itemA.m_addInfo == 0 || itemB.m_addInfo == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 4);
        return 0;
    }
    if (!(itemA.m_addInfo == (int)seqA && itemB.m_addInfo == (int)seqB))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 1);
        return 0;
    }

    if (chronicle->IsEmancipate() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0x11);
        return 0;
    }
    if (equipItem->is_stackable())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 1);
        return 0;
    }
    CEquipItem* equip = static_cast<CEquipItem*>(equipItem);
    if (equip == 0 || equip->IsEmancipate() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0x11);
        return 0;
    }

    int start = 0;
    int end = 0;
    inven->getStartEndPos(Inven_Item::ITEM_TYPE_EQUIP, start, end);
    if (slotB < start || end < slotB)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0x13);
        return 0;
    }
    const int endurance = equip->get_endurance();
    if ((int)itemB.m_fieldb < endurance)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 7);
        return 0;
    }

    stAmplifyOption_t& ampView = itemB.m_amp;   // ORIG asStack_f1 即 itemB 内嵌 amp 视图
    (void)ampView.getAbilityType();             // local_21：调用后弃用（ORIG 留痕）
    if (!ampView.isIdentified())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0xae);
        return 0;
    }

    std::vector<int> updated;
    int rc = G_CDataManager()->m_itemUpgrade->ProcUpgradeChronicle(
        user, slotA, seqA, slotB, seqB, chanceGrade, updated);
    if (rc != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x112, 0xae);
        return 0;
    }
    if (updated.size() % 3 == 0)
    {
        PacketGuard packetOut;
        packetOut.put_header(1, 0x112);
        packetOut.put_byte(1);
        packetOut.put_byte((int)((char)(updated.size() / 3) + '\x01'));
        inven->GetInvenSlot(1, (int)slotB);   // ORIG 第二次取槽（写包前置读取）
        packetOut.put_byte(0);
        packetOut.put_short((int)slotA);
        packetOut.put_int(1);
        for (std::vector<int>::const_iterator vIt = updated.begin();
             vIt != updated.end(); vIt++)   // ORIG 增量段 ppEi（P28 后置形）
        {
            packetOut.put_byte((int)(char)*vIt);
            vIt++;   // ORIG ppEi（P28 后置形）
            packetOut.put_short((int)(short)*vIt);
            vIt++;
            packetOut.put_int(*vIt);
        }
        packetOut.finalize(true);
        user->Send(packetOut);
    }
    return 0;
