int Inter_GuildCargoCheckPushItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoCheckPushItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoCheckPushItemPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $0x2,setle → state>2 才放行
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_e)          // msg+0x0e characNo
        return 0;

    if (pPkt->m_12 != 0xc1)   // 失败结果上报分支
    {
        user->m_historyLog.GuildCargoCheckPushItemResult(
            (ENUM_GUILD_CARGO_HISTORY_REASON)6,
            pPkt->m_12,
            pPkt->m_13,
            pPkt->m_17,
            pPkt->m_1b,
            pPkt->m_24,
            pPkt->m_25);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb,
                                 pPkt->m_12);
        return 0;
    }

    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(   // -0x71 栈槽
        (int)GetInvenTypeFromItemSpace(
            (ENUM_ITEMSPACE)pPkt->m_24),  // msg+0x24 itemSpace
        (int)pPkt->m_25);    // msg+0x25 slot

    if (pPkt->m_17 == 0 ||
        item.m_addInfo != pPkt->m_17)   // 客户端槽位核对
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);      // -0x28
    if (ci == 0 || ci->isPackagable() != 1 || ci->GetCashPrice() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    if (ci->IsImpossibleCharacCargo())                 // ORIG vtable+0x40 槽直调
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    if (IsTradeLimitAttachTypeItem(item))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    if (ci->getUsablePeriod() != 0 || ci->getExpirationDate() != 0)
    {
        const int expireSec = (int)item.m_fieldb * 0x15180 + 0x44a54a80;
        if (expireSec <= GlobalData::s_systemTime_.getCurSec())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
            return 0;
        }
    }

    if (ci->is_stackable() != 0 &&
        pPkt->m_1b > item.m_addInfo2)   // 堆叠数不足
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc6);
        return 0;
    }

    const int attachType = ci->GetAttachType();
    if (attachType == 1 || attachType == 2 || attachType == 5 ||
        (attachType == 3 && item.m_field0 != 1) ||
        item.m_upgradeSep.IsTradeRestriction())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    if (user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)item.GetInventoryType(),
            pPkt->m_25,
            pPkt->m_1b,
            (eItemDelReason)0x22, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    user->SaveInventory();

    Packet_Guild_Guild_Cargo_Push_Item pck;            // ORIG ctor 0x084e9434
    DnfItemInfoView& pckItem = *reinterpret_cast<DnfItemInfoView*>(&pck.m_item);  // +0x1b DnfItemInfo 视图（头内 m_item 为字节区）
    pck.m_guildKey = (unsigned int)user->get_charac_guildkey();   // +0x0a
    pck.m_charNo   = (unsigned int)user->getCurCharacNo();        // +0x0e
    pck.m_b        = (int)pPkt->m_13;   // +0x12
    pck.m_d        = pPkt->m_1f;   // +0x16
    pckItem.m_itemId     = (unsigned int)item.m_addInfo;       // +0x1c
    pckItem.m_attr       = item.GetItemAttr();                 // +0x20
    pckItem.m_addInfo    = (unsigned int)item.m_addInfo2;      // +0x21
    pckItem.m_endurance  = item.m_fieldb;                      // +0x25
    pckItem.m_extendInfo = (unsigned int)item.m_fieldd;        // +0x27
    item.m_amp.getValues(pckItem.m_abilityType, pckItem.m_abilityValue);
    // p167：ORIG 无 memcpy call ⇒ 结构整体赋值形（N84/P83）。
    *reinterpret_cast<RandomOption*>(&pckItem.m_opt) =    // +0x1d ← item+0x25
        *reinterpret_cast<const RandomOption*>(
            reinterpret_cast<const unsigned char*>(&item) + 0x25);
    pckItem.m_up.SetUpgradeSeparate(item.m_upgradeSep.GetUpgradeSeparate());
    pck.m_e = pPkt->m_24;     // +0x50
    pck.m_f = pPkt->m_25;    // +0x51
    if (ci->is_stackable() != 0)                                  // 堆叠件记录申请数
        pck.m_d     = (unsigned int)pPkt->m_1b;  // +0x16 覆写
    else
        pck.m_d     = (unsigned int)item.m_addInfo2;

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pck), (int)pck.m_f);

    user->m_historyLog.GuildCargoPushItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)7,
        (int)pck.m_b, (int)pckItem.m_itemId, (int)pck.m_d,
        (int)pckItem.m_endurance, (int)pckItem.m_extendInfo,
        (int)pckItem.m_attr, pckItem.m_up.GetUpgradeSeparate());
    return 0;
