int Dispatcher_GuildCargoMoveItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stGuildCargoMoveItemParam& out = static_cast<stGuildCargoMoveItemParam&>(param);
    out.m_4 = check_error(user, msg, param);                    // ORIG 0x081d18dc
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        LineFunc(0x2dff,
                 __PRETTY_FUNCTION__,
                 out.m_4, 0);
        return 0;
    }

    MSG_GUILD_CARGO_MOVE_ITEM& req = reinterpret_cast<MSG_GUILD_CARGO_MOVE_ITEM&>(msg);

    Packet_Guild_Guild_Cargo_Move_Item pkt;
    pkt.m_guildKey = user->get_charac_guildkey();               // → +0x0a
    pkt.m_charNo = user->getCurCharacNo();                      // → +0x0e
    pkt.m_fromSlot = (int)req.m_field0d;                        // msg+0x0d ushort → +0x12
    pkt.m_toSlot = (int)req.m_field0f;                          // msg+0x0f ushort → +0x16
    pkt.m_count = req.m_field11;                                // msg+0x11 → +0x1a
    pkt.m_type = req.m_field15;                                 // msg+0x15 → +0x1e

    CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(
        user->GetServerGroup());
    proxy->SendTcpPacket((char*)&pkt, (int)pkt.packetSize);     // 0x22

    user->m_historyLog.GuildCargoMoveItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xd,
        (int)req.m_field0d, (int)req.m_field0f);                // ORIG 0x086867ce
    return 0;
