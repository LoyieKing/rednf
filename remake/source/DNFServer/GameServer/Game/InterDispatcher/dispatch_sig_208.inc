// [b28d-lane2] verdict=DIFF ndiff=291（ORIG 0x084ddea2）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+6。
// [b28d-solo-InterDispatcher] verdict=DIFF（ORIG 0x84ddea2）：本轮 memcpy→RandomOption 结构赋值（N84/P83，p167 CLEAR）；余 N22/N18a/P30。
int Inter_GuildCargoPushItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮负结论 N，ndiff=374（ratio 0.401，diff 对齐失真）] 逐段核对后，本件差异
    //   集中在同一族指令选择：ORIG `mov 0x18(%eax),%eax; mov %eax,%ebx`（两条）vs 本方
    //   `mov 0x18(%eax),%ebx`（一条）、`mov 0x17(%eax),%eax; mov %eax,%edx` vs `…,%edx`，
    //   即 ORIG 多处反而多一条 regmove —— gcc 伪寄存器分派差，非源码形态差；
    //   两侧 call 序列与实参布局一致。无可单点判定项，留 N。
    Inter_GuildCargoPushItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoPushItemPktView*>(data);
    if (user->get_state() <= 2)                        // cmp $0x2,setle
        return 0;
    if (user->get_charac_no(-1) !=
        pPkt->m_e)
        return 0;

    user->m_historyLog.GuildCargoPushItemResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)8,
        pPkt->m_12,
        pPkt->m_13,
        pPkt->m_18,
        pPkt->m_1d,
        pPkt->m_21,
        pPkt->m_23);

    if (pPkt->m_12 == 0xc1)
    {   // 公会服务器确认成功
        PacketGuard packet;                            // -0x30
        packet.put_header(1, 0xfb);
        packet.put_byte(1);
        packet.put_byte((int)pPkt->m_4c);
        packet.put_short((int)pPkt->m_4d);
        packet.put_int(pPkt->m_1d);
        packet.put_short(pPkt->m_13);
        packet.finalize(true);
        user->Send(packet);
        return 0;
    }

    user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfb,
                             pPkt->m_12);

    CInventory* inven = user->getCurCharacInvenW();    // -0x20
    CItem* ci = G_CDataManager()->find_item(           // -0x1c
        pPkt->m_18);

    Inven_Item item;                                   // -0x6d（默认构造）
    ci->make_item(item);                               // ORIG vtable+0x08 槽直调

    item.m_addInfo = pPkt->m_18;
    item.m_addInfo2 = pPkt->m_1d;
    item.m_fieldb = pPkt->m_21;   // endurance
    item.m_fieldd = pPkt->m_23;
    item.SetItemAttr(pPkt->m_1c);
    item.m_amp.assign(pPkt->m_27,
                      pPkt->m_28);
    // RandomOption ← msg+0x34（+0x25 = m_random.m_random 连续区）
    // p167：ORIG 无 memcpy call ⇒ 结构整体赋值形（N84/P83）。
    item.m_random.m_random = *reinterpret_cast<const RandomOption*>(data + 0x34);
    item.m_upgradeSep.SetUpgradeSeparate(
        pPkt->m_42.GetUpgradeSeparate());

    const int slot = inven->insertItemIntoInventory(   // ORIG 0x08502d86
        item, (eItemAddReason)0x27, true, false);

    if (slot < 0)
    {   // 入包失败：系统邮件回收 + 恢复日志
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(4, "game_server_msg_114", 0),
            item, 0,
            pPkt->m_e,
            reinterpret_cast<const char*>(0), 0, 0,
            (ENUM_SERVER_GROUP)user->GetServerGroup(), false, false);
        user->m_historyLog.GuildCargoItemRecovery(
            (ENUM_GUILD_CARGO_HISTORY_REASON)9,
            pPkt->m_18,
            pPkt->m_1d,
            (int)pPkt->m_21,
            pPkt->m_23,
            (int)pPkt->m_1c,
            pPkt->m_42.GetUpgradeSeparate());
    }
    return 0;
}