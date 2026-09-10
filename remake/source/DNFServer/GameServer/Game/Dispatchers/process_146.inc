int Dispatcher_GuildCargoPopItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stGuildCargoPopItemParam& out = static_cast<stGuildCargoPopItemParam&>(param);
    out.m_4 = check_error(user, msg, param);            // ORIG 0x081d14aa
    if (out.m_4 > 0)
    {
        return 0;                                        // jle 取反分支：err>0 直接返回
    }
    if (out.m_4 < 0)
    {
        LineFunc(0x2d8f,
                 __PRETTY_FUNCTION__,
                 out.m_4, 0);
        return 0;
    }

    MSG_GUILD_CARGO_POP_ITEM& req = reinterpret_cast<MSG_GUILD_CARGO_POP_ITEM&>(msg);
    CItem* item = G_CDataManager()->find_item(req.m_itemIdx);   // msg+0x0f

    Packet_Guild_Guild_Cargo_Pop_Item pkt;
    pkt.m_guildKey = user->get_charac_guildkey();               // → +0x0a
    pkt.m_charNo = user->getCurCharacNo();                      // → +0x0e
    pkt.m_slot = (int)req.m_field0d;                            // msg+0x0d ushort → +0x12
    pkt.m_id = req.m_itemIdx;                                   // msg+0x0f → +0x16
    pkt.m_count = req.m_field13;                                // msg+0x13 → +0x1a
    pkt.m_itemType = item->is_stackable() ? 1 : 0;              // CItem::is_stackable → +0x1e
    pkt.m_uit = req.m_field17;                                  // msg+0x17 → +0x1f

    // [G1Dproc 2026-09-10] ORIG 0x081d16b8（`movzwl -0x45(%ebp),%eax` = pkt+0x02 长度 →
    //   %esi；`lea -0x47(%ebp),%ebx` = &pkt；this 链最后求值）⇒ 无 `proxy` 具名槽（P0）。
    //   长度 = *(ushort*)(pkt+2) = 0x23
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket((char*)&pkt, (int)pkt.packetSize);

    user->m_historyLog.GuildCargoPopItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xa,
        (int)req.m_field0d, req.m_itemIdx, req.m_field13,
        (int)pkt.m_itemType, (int)req.m_field17);               // ORIG 0x086866ea
    return 0;
