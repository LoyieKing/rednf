// [b28d-lane2] verdict=DIFF ndiff=404（ORIG 0x084dd856）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-13。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84dd856）：本轮 memcpy→RandomOption 结构赋值（N84/P83，p167 CLEAR）；余 N22/N18a/P30。
int Inter_GuildCargoCheckPushItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮负结论 N，ndiff=146] 残留为混合残差，无单点可判定项：
    //   (a) ORIG 多条 `mov 0xNN(%eax),%eax; mov %eax,%ebx` / `mov %eax,%esi`（ORIG 反而
    //       比本方多一条 regmove）vs 本方单条 `mov 0xNN(%eax),%ebx` —— gcc 伪寄存器分派差；
    //   (b) 本方结构体整体赋值（`mov %edx,0x4(%eax)`、逐字段 `mov %ecx,0x1c(%eax)`）与
    //       ORIG 逐字段槽往返形互异；
    //   (c) `comisd` 类比较的 `jg` vs `setge/test/je`（bool 物化）两侧相反。
    //   以上均需逐字段直证才能动，留 N。
    Inter_GuildCargoCheckPushItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoCheckPushItemPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $0x2,setle → state>2 才放行
        return 0;

    if (user->get_charac_no(-1) !=
        pPkt->m_characNo)          // msg+0x0e characNo
        return 0;

    if (pPkt->m_result != 0xc1)   // 失败结果上报分支
    {
        user->m_historyLog.GuildCargoCheckPushItemResult(
            (ENUM_GUILD_CARGO_HISTORY_REASON)6,
            pPkt->m_result,
            pPkt->m_13,
            pPkt->m_addInfo,
            pPkt->m_addInfo2,
            pPkt->m_itemSpace,
            pPkt->m_slot);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb,
                                 pPkt->m_result);
        return 0;
    }

    // [c3] ORIG 0x084dd9c6..0x084dda1e：receiver 先求值（-0x30 槽）→ 再算实参
    // （GetInvenTypeFromItemSpace 结果 -0x2c 槽）⇒ 具名 receiver + 具名 invenType。
    const CInventory* invenR = user->getCurCharacInvenR();        // -0x30 槽（ORIG）
    const int invenType = (int)GetInvenTypeFromItemSpace(
        (ENUM_ITEMSPACE)pPkt->m_itemSpace);  // msg+0x24 itemSpace
    Inven_Item item = invenR->GetInvenSlot(invenType,             // -0x71 栈槽
        (int)pPkt->m_slot);    // msg+0x25 slot

    if (pPkt->m_addInfo == 0 ||
        item.m_addInfo != pPkt->m_addInfo)   // 客户端槽位核对
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
        pPkt->m_addInfo2 > item.m_addInfo2)   // 堆叠数不足
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
            pPkt->m_slot,
            pPkt->m_addInfo2,
            (eItemDelReason)0x22, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb, 0xc5);
        return 0;
    }

    user->SaveInventory();

    Packet_Guild_Guild_Cargo_Push_Item pck;            // ORIG ctor 0x084e9434
    pck.m_guildKey = (unsigned int)user->get_charac_guildkey();   // +0x0a
    pck.m_charNo   = (unsigned int)user->getCurCharacNo();        // +0x0e
    // [c3] ORIG 0x084ddcbf..0x084ddcc6：`movzbl 0x23(%eax),%eax; mov %al,-0xaa(%ebp)`
    // （-0xaa = pck+0x1a）⇒ m_itemType 来自 pPkt+0x23（旧形完全缺失）。
    pck.m_itemType = pPkt->m_itemType;   // +0x1a
    pck.m_b        = (int)pPkt->m_13;   // +0x12
    pck.m_d        = pPkt->m_1f;   // +0x16
    // [c3] ORIG 0x084ddce4..0x084ddcf6：m_f（+0x51，word）先于 m_e（+0x50，byte），
    // 且紧接 m_d 之后 ⇒ 与下方 item 字段段分开写。
    pck.m_f = pPkt->m_slot;    // +0x51
    pck.m_e = pPkt->m_itemSpace;     // +0x50
    // [c3] pck.m_item 为真实 DnfItemInfo 成员（头内已字段化）⇒ 逐字段栈常量写
    // （ORIG 0x084ddcf9..0x084ddd53 段为 pck+0x1c/+0x21/+0x25/+0x27/+0x20 直写）。
    pck.m_item.m_itemId     = (unsigned int)item.m_addInfo;       // +0x1c
    pck.m_item.m_addInfo    = (unsigned int)item.m_addInfo2;      // +0x21
    pck.m_item.m_endurance  = item.m_fieldb;                      // +0x25
    pck.m_item.m_extendInfo = (unsigned int)item.m_fieldd;        // +0x27
    pck.m_item.m_attr       = item.GetItemAttr();                 // +0x20（ORIG 在 extendInfo 之后）
    item.m_amp.getValues(pck.m_item.m_abilityType, pck.m_item.m_abilityValue);
    // [c3] ORIG 0x084ddd58..0x084ddd77：pck+0x38（= m_opt）14B 块（4/4/4/2 段），
    // 源为 item+0x25 的 disp 直读 ⇒ 用真实成员 m_random.m_random（去 cast+算术）。
    pck.m_item.m_opt = item.m_random.m_random;
    pck.m_item.m_up.SetUpgradeSeparate(item.m_upgradeSep.GetUpgradeSeparate());
    // [c3] ORIG 0x084dddb0..0x084dddc1 覆写目标是 pck+0x21（= pckItem.m_addInfo，
    // `mov [ebp-0xa3],eax`；-0xa3 = pck+0x21），非 pck+0x16（m_d 只写一次）。
    if (ci->is_stackable() != 0)                                  // 堆叠件记录申请数
        pck.m_item.m_addInfo = (unsigned int)pPkt->m_addInfo2;  // +0x21 覆写
    else
        pck.m_item.m_addInfo = (unsigned int)item.m_addInfo2;

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    // [DNF-FIX 2026-10-08 c31] ORIG 0x084dddc7 `movzwl -0xc2` = pck(-0xc4)+2 = packetSize（旧码误用 m_f +0x51 槽号）
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pck), (int)pck.packetSize);

    user->m_historyLog.GuildCargoPushItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)7,
        (int)pck.m_b, (int)pck.m_item.m_itemId, (int)pck.m_d,
        (int)pck.m_item.m_endurance, (int)pck.m_item.m_extendInfo,
        (int)pck.m_item.m_attr, pck.m_item.m_up.GetUpgradeSeparate());
    return 0;
}