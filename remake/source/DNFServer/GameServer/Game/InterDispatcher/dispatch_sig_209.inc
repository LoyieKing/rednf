int Inter_GuildCargoPopItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
Inter_GuildCargoPopItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoPopItemPktView*>(data);
    // ORIG 0x084de330 报告 docs/class_func_reports/Inter_GuildCargoPopItem/dispatch_sig.md
    // 两段全读。msg 布局：+0x0e characNo / +0x12 resultByte / +0x17 itemIdx /
    // +0x1c addInfo2 / +0x20 endurance(u16) / +0x22 fieldd / +0x1b SetItemAttr /
    // +0x26..0x27 amp.assign / +0x33..0x41 RandomOption 区 15B 块 / +0x4b errByte /
    // +0x4c logIdx。
    if (user->get_state() <= 2)
        return 0;
    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    user->m_historyLog.GuildCargoPopItemResult(
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xb,
        pPkt->m_4b,
        pPkt->m_4c,
        pPkt->m_17,
        pPkt->m_1c,
        pPkt->m_20,
        pPkt->m_22);

    if (pPkt->m_4b != 0xc1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfc,
                                 pPkt->m_4b);
        return 0;
    }

    CInventory* inven = user->getCurCharacInvenW();
    CDataManager* dm = G_CDataManager();
    Inven_Item item;                       // ORIG 局部 Inven_Item C1 @ebp-0x85
    CItem* ci = dm->find_item(pPkt->m_17);
    if (ci == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x4a6e, 0)("ERROR OCCURRED! GUILD CARGO POP ITEM - fail find item(%d), User(%d)",
              pPkt->m_17, user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xfc, 0xc5);
        return 0;
    }

    ci->make_item(item);                   // vtable 槽 +0x08（make_item 出参）
    item.m_addInfo = pPkt->m_17;
    item.m_addInfo2 = pPkt->m_1c;
    item.m_fieldb = pPkt->m_20;
    item.m_fieldd = pPkt->m_22;
    item.SetItemAttr(pPkt->m_1b);
    item.m_amp.assign(pPkt->m_26,
                      pPkt->m_27);
    // RandomOption 连续块（item+0x25..0x32）与升级分离位（item+0x33）：
    // 网络消息到跨字段连续区的按块拷贝（ORIG 五段 disp 存储指令；
    // 同款按块视图先例 CMailBoxHelper.cpp:514 sig_at(&item,0x25)）。
    // +0x25..+0x32 = m_random.m_random（ChronicleRandomOptionInfo+0x06）连续区块写
    *(unsigned int*)&item.m_random.m_random.m_field0 =
        pPkt->m_33;
    *(unsigned int*)&item.m_random.m_random.m_field3 =
        pPkt->m_37;
    *(unsigned int*)&item.m_random.m_random.m_field6 =
        pPkt->m_3b;
    *(unsigned short*)&item.m_random.m_random.m_seed9 =
        pPkt->m_3f;
    item.m_upgradeSep.m_field = pPkt->m_41;

    int firstResult = 0;                   // 试占位结果（包内回发槽号）
    if (ci->IsCreatureItem())              // vtable 槽 +0x14
        firstResult = inven->tryInsertItemIntoCreature(item);
    else if (ci->IsAvatarItem())           // vtable 槽 +0x10
        firstResult = inven->tryInsertAvatarIntoInventory(item);
    else
        firstResult = inven->tryInsertItemIntoInventory(item);

    if (firstResult < 0)
    {
        const char* title =
            g_scriptStringManager_.findString(4, "game_server_msg_114",
                                              reinterpret_cast<bool*>(0));
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            title, item, 0u, static_cast<unsigned int>(pPkt->m_e),
            reinterpret_cast<const char*>(0), 0, 0u, user->GetServerGroup(),
            false, false);
        user->m_historyLog.GuildCargoItemRecovery(
            (ENUM_GUILD_CARGO_HISTORY_REASON)0xc,
            pPkt->m_17,
            pPkt->m_1c,
            pPkt->m_20,
            pPkt->m_22,
            pPkt->m_1b,
            (*reinterpret_cast<UpgradeSeparateInfo*>(&pPkt->m_41))   // §4.3 具名字段取址替代裸偏移 data+0x41
                .GetUpgradeSeparate());
    }
    else
    {
        int second = 0;
        if (ci->IsCreatureItem())
            second = inven->insertItemIntoCreature(
                item, (eItemAddReason)0x28, true, false);
        else if (ci->IsAvatarItem())
            second = inven->insertAvatarIntoInventory(
                item, (eAvatarItemAddReason)0x12, true);
        else
            // ORIG 0x084de1c7：普通物品实际插入（insertItemIntoInventory，
            // reason=0x28/bLog=true/b=false）；NEW 原 tryInsert* 成功后
            // 无实际插入分支 → 物品丢失，属转写缺失。
            second = inven->insertItemIntoInventory(
                item, (eItemAddReason)0x28, true, false);
        if (second < 0)
        {
            const char* title =
                g_scriptStringManager_.findString(4, "game_server_msg_114",
                                                  reinterpret_cast<bool*>(0));
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, item, 0u,
                static_cast<unsigned int>(pPkt->m_e),
                reinterpret_cast<const char*>(0), 0, 0u, user->GetServerGroup(),
                false, false);
            user->m_historyLog.GuildCargoItemRecovery(
                (ENUM_GUILD_CARGO_HISTORY_REASON)0xc,
                pPkt->m_17,
                pPkt->m_1c,
                pPkt->m_20,
                pPkt->m_22,
                pPkt->m_1b,
                (*reinterpret_cast<UpgradeSeparateInfo*>(&pPkt->m_41))   // §4.3 具名字段取址替代裸偏移 data+0x41
                    .GetUpgradeSeparate());
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xfc);
    packet.put_byte(1);
    packet.put_short(pPkt->m_4c);
    packet.put_byte(pPkt->m_12);
    packet.put_short(firstResult);
    packet.put_int(pPkt->m_1c);
    packet.finalize(true);
    user->Send(packet);
    return 0;
