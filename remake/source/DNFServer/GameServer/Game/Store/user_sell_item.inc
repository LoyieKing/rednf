int Store::user_sell_item(CUser* user, char invenType, short slot, short count)
{
    if (user->CheckInTrade())
    {
        return 0x13;
    }

    int price = 0;
    int ret = 0;
    int outCount = 0;
    ret = GetSellItemPrice(user, invenType, slot, count, user->get_state() == 5, price, outCount);
    if (ret != 0)
    {
        return ret;
    }

    if (!user->CheckMoney(outCount))
    {
        return 0x16;
    }

    Inven_Item item;
    // ORIG 0x086194ea：case 0/3/7 共用 GetInvenTypeFromItemSpace 路，case 2 走 cargo。
    switch (invenType)
    {
    case 0:
    case 3:
    case 7:
        item = user->getCurCharacInvenR()->GetInvenSlot(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)invenType), slot);
        break;
    case 2:
        item = user->getCurCharacCargoR()->get_cargo_slot(slot);
        break;
    default:
        break;
    }

    // ORIG 0x08619708：delete_item 分派同 switch（case 升序 0/2/3/7）。
    switch (invenType)
    {
    case 0:
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, count, eItemDelReason_0, true))   // 消息 0 → type 1
        {
            cMyTrace("int Store::user_sell_item(CUser*, char, short int, short int)", 0x136, 5)
                ("Store::user_sell_item, delete_item failed , User ch=%d , %d %d",
                 user->getCurCharacNo(), slot, count);
            return 0x11;
        }
        break;
    case 2:
        if (!user->getCurCharacCargoW()->delete_item(slot, count, eItemDelReason_0))
        {
            cMyTrace("int Store::user_sell_item(CUser*, char, short int, short int)", 0x141, 5)
                ("Store::user_sell_item, delete_item failed 2, User ch=%d , %d %d",
                 user->getCurCharacNo(), slot, count);
            return 0x11;
        }
        break;
    case 3:
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)0, slot, 1, eItemDelReason_0, true))   // 消息 3 → type 0
        {
            cMyTrace("int Store::user_sell_item(CUser*, char, short int, short int)", 0x14d, 5)
                ("Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
                 user->getCurCharacNo(), slot);
            return 0x11;
        }
        user->send_equip(slot);
        break;
    case 7:
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)3, slot, count, eItemDelReason_0, true))   // 消息 7 → type 3
        {
            cMyTrace("int Store::user_sell_item(CUser*, char, short int, short int)", 0x167, 5)
                ("Store::user_sell_item, delete_item failed 4, User ch=%d , %d %d",
                 user->getCurCharacNo(), slot, count);
            return 0x11;
        }
        break;
    default:
        break;
    }

    if (outCount > 0x7a11f)
    {
        // ORIG 0x08619a23：get_money() 结果直接留 %edi（无槽往返）⇒ 无具名局部（P0），
        // 内联为 trace 最右实参（右→左求值序）。
        cMyTrace("int Store::user_sell_item(CUser*, char, short int, short int)", 0x172, 4)
            ("User(%s) Charac(%s) gain money(%d) by selling item(%d), old money is %d.",
             NumberToString(user->get_acc_id(), 0),
             user->getCurCharacName(), outCount, price,
             user->getCurCharacInvenR()->get_money());
    }

    user->getCurCharacInvenW()->gain_money(outCount, eMoneyAddReason_0, true, 0);

    if (!item.isEmpty())
    {
        CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
        // ORIG 0x08619b1a：两条独立守卫各自 `je <Add_RedeemInfo>` 直跳（P133），
        // 合并 && 会被 4.4 物化成 setne/sete 序列。
        if (pItem != 0)
        {
            if (pItem->is_stackable())
                item.m_addInfo2 = count;   // ORIG movswl count → item+0x7（stackable 覆写数量）
        }
        user->Add_RedeemInfo(item, outCount, true);
    }

    PacketGuard packet;
    packet.put_header(1, 0x18);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_byte(invenType);
    packet.put_short(slot);
    packet.put_short(count);
    packet.finalize(true);
    user->Send(packet);

    // ORIG 0x08619c43/0x08619c4c：两个函数均静态调用（无 this）；IsInvitationPaper
    // 返回类型 bool（ORIG `call; test %al,%al; je` 无 setne，P292/P315）。
    if (CHackLog_InvitationPaperSellToNPC::IsInvitationPaper(price))
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)CHackLog_InvitationPaperSellToNPC::GetHackType(),
            1, 0, 0);
    }
    HistoryLog::WriteSellItem((FILE*)user->m_field796f8, price, count);

    return 0;
