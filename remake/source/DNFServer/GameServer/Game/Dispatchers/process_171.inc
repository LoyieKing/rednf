int Dispatcher_GuildCargoPushItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d10d4 (0x242 insn)
    stGuildCargoPushItemParam& result = static_cast<stGuildCargoPushItemParam&>(param);
    const MSG_GUILD_CARGO_PUSH_ITEM* const M =
        reinterpret_cast<const MSG_GUILD_CARGO_PUSH_ITEM*>(&msg);

    result.m_4 = check_error(user, msg, param);         // +0x04 错误码
    if (result.m_4 >= 1)
        return 0;
    if (result.m_4 < 0)
        return LineFunc(0x2cf0, __PRETTY_FUNCTION__, result.m_4, 0);

    // [round5 A01] P0/P183：ORIG 0x081d1163/0x081d1183/0x081d11b5 段**无** itemSpace/slot/
    //   dataMgr 具名槽（M 是唯一 -0x28 具名指针；G_CDataManager() 内联进 find_item 的 receiver）
    //   ⇒ 三处内联，消 3 组 store/reload 对（我方 pairs 2 vs ORIG 0）。
    const CInventory* inven = user->getCurCharacInvenR();
    const int invenType =
        GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(unsigned char)M->m_field0d);
    const Inven_Item slotItem =
        inven->GetInvenSlot(invenType, static_cast<int>(M->m_field0e));
    CItem* item = G_CDataManager()->find_item(slotItem.m_addInfo);

    // ---- ORIG 0x081d11c9..0x081d1305（Fix7_I 恢复真实调用）----
    // ORIG is_stackable 读两次（P48）：一次存 bool、一次作分支判据。
    const bool isStackable = item->is_stackable();
    const int stackLimit =
        item->is_stackable() ? static_cast<CStackableItem*>(item)->getStackableLimit() : 1;

    Packet_Guild_Check_Guild_Cargo_Push_Item pkt;            // ctor 0x0822bec2
    pkt.m_guildKey = (unsigned int)user->get_charac_guildkey();   // +0xa
    pkt.m_charNo = (unsigned int)user->getCurCharacNo();          // +0xe
    pkt.m_slot = (int)M->m_field18;                              // +0x12
    pkt.m_itemId = M->m_field10;                                 // +0x16
    pkt.m_count = M->m_field14;                                  // +0x1a
    pkt.m_maxStack = stackLimit;                                  // +0x1e
    pkt.m_stackable = (unsigned char)(isStackable ? 1 : 0);       // +0x22
    pkt.m_invenType = M->m_field0d;                              // +0x23
    pkt.m_invenSlot = M->m_field0e;                              // +0x24

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt),
                         (int)*(unsigned short*)((char*)&pkt + 2));

    user->m_historyLog.GuildCargoCheckPushItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)5,
        (int)M->m_field18,
        M->m_field10,
        M->m_field14,
        (int)M->m_field0d,
        (int)M->m_field0e,
        (int)(isStackable ? 1 : 0));
    return 0;
