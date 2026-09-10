bool CUser::Redeem_Item(int redeemId)
{
    // [b28d-solo-UserMiscLink] 判词（N 封顶）：301/293、p167 CLEAR(44=44)；
    // 残差=①4.4 局部槽基整体深 4（redeem -0x1c vs -0x18、info -0xd0 vs -0xcc；
    // 引用改指针无效）⇒ N95/N82；②4.4 位折叠 `(unsigned)price>>31` 折成 cmpl/jns
    // （ORIG `shr $0x1f; test %al,%al`）⇒ N6/N99。P198① 预置 -1 与 P221 trace 临时链已复现。
    // GetCharacExpandData(6) 即 Redeem_Item::CRedeemItem（槽位 6，CUser.cpp 同型 cast）
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (redeem == 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(this))
    {
        // [b28d-solo-UserMiscLink] P221/P227：两个互斥分支的 cMyTrace 改全表达式临时链，
        // 4.4 具名对象恒「先声明深→浅」与 ORIG（-0x7c/-0x6c）相反。
        cMyTrace(__PRETTY_FUNCTION__, 0x529, 5)(
            "redeem error : trade : m_id(%s)charac_no(%u)",
            NumberToString(get_acc_id(), 0), (unsigned int)get_charac_no(-1));
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (CheckInTrade())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x52f, 5)(
            "redeem error : trade : m_id(%s)charac_no(%u)",
            NumberToString(get_acc_id(), 0), (unsigned int)get_charac_no(-1));
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    // [b28d-solo-UserMiscLink] ORIG 0x08646f97：`movl $0xffffffff,-0x14` 预置 +
    // 独立赋值（P198①）；负值判定为 `shr $0x1f,%eax; test %al,%al; je`（4.1 无符号移位形）。
    int price = -1;
    price = redeem->GetItemPrice(redeemId);
    if (((unsigned int)price >> 31) != 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    if (CUserCharacInfo::getCurCharacMoney() < price)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 0x46);
        return 0;
    }

    Redeem_Item::Inven_Item_Expand_RedeemInfo info = redeem->GetRedeemInfoR(redeemId);
    // info 前 0x3d 字节即 Inven_Item 快照（AddRedeemList 复制口径）；m_item 为
    // 真实 Inven_Item 成员（pack(1) 布局一致；ORIG 以具名指针 local_14(-0x10) 消费，
    // 引用形会多占一槽使全帧深 4）。
    Inven_Item* item = reinterpret_cast<Inven_Item*>(&info.m_item);
    if (item->isEmpty())
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 3);
        return 0;
    }

    int slot = CUserCharacInfo::getCurCharacInvenW()->insertItemIntoInventory(
        *item, (eItemAddReason)0x29, true, true);
    if (slot < 0)
    {
        SendCmdErrorPacket((ENUM_CMDPACKET)0x137, 4);
        return 0;
    }

    CUserCharacInfo::getCurCharacInvenW()->use_money(price, (eMoneySubReason)0x25, true);

    CItem* pItem = G_CDataManager()->find_item(item->m_addInfo);
    // 非堆叠 → 历史记录 count=1；堆叠 → count=item.m_addInfo2(+0x07)。
    if (pItem == 0 || !pItem->is_stackable())
    {
        m_historyLog.RedeemItemDel(item->m_addInfo, 1);
    }
    else
    {
        m_historyLog.RedeemItemDel(item->m_addInfo, item->m_addInfo2);
    }

    redeem->RedeemItem(redeemId);

    PacketGuard packet;
    packet.put_header(1, 0x137);
    packet.put_byte(1);
    packet.put_int(slot);
    packet.finalize(true);
    Send(packet);
    return 1;
