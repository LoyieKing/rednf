// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08223330 orig/ours=538/524 ndiff-blocks=91
//   帧差 sub $17c vs $16c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x17c,%esp | movw   $0x0,-0x4e(%ebp) | movl   $0x0,-0x54(%ebp)
// [SweepDispatchers3 2026-09-09] 修真缺口：vector<int> 遍历增量改后置 vIt++（P28：
//   ORIG 增量段 ppEi×3，p167 MISSING ppEi×3 / EXTRA ppEv×3 归零）。gap 13→9
//   （528 vs 537），残差 = 4.1 码风（je/movl/lea/xor 各 2-3）。
int Dispatcher_UpgradeChronicle::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08223330：三代编年史强化（WongWork::CItemUpgrade::ProcUpgradeChronicle）。
    if (user->get_state() < 3)
        return 0;

    // [G2-83 2026-09-11 · P212] ORIG 0x08223330 段五条清零**连续**（`movw $0,-0x4e`/
    //   `movl $0,-0x54`/`movw $0,-0x56`/`movl $0,-0x5c`/`movb $0,-0x5d`），随后才
    //   `lea -0x4e(%ebp),%eax` 取首目标 ⇒ 原码五局部同块连写声明（非逐声明夹 if）。
    short slotA = 0;
    unsigned int seqA = 0;
    short slotB = 0;
    unsigned int seqB = 0;
    char chanceGrade = 0;
    if (!packet.get_short(slotA))
        return LineFunc(0xdda4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(seqA))
        return LineFunc(0xdda5,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(slotB))
        return LineFunc(0xdda6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(seqB))
        return LineFunc(0xdda7,
                        __PRETTY_FUNCTION__,
                        0, 0);
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
    // [bughunt9] ORIG 0x08223777 `movl $0x0,0x4(%esp)`：ITEM_TYPE_0（槽 [3,8]），
    //   0x08223796 `jl ok` / 0x082237a4 `jg ok`，落空才 0x13 ⇒ 槽位落在 [3,8] 内才拒绝
    //   （与 CItemUpgrade::ProcUpgrade 0x0854a39c 同形）。旧写 ITEM_TYPE_EQUIP + `||`
    //   方向全反（只放行 [9,0x38]）。
    inven->getStartEndPos((Inven_Item::ITEM_TYPE)0, start, end);
    if (slotB >= start && slotB <= end)
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
}