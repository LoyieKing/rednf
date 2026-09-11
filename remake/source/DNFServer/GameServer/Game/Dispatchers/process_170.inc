// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d1954 orig/ours=93/94 ndiff-blocks=18
//   帧差 sub $40 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x40,%esp
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
        return LineFunc(0x2dff,
                        __PRETTY_FUNCTION__,
                        out.m_4, 0);
    }

    MSG_GUILD_CARGO_MOVE_ITEM& req = reinterpret_cast<MSG_GUILD_CARGO_MOVE_ITEM&>(msg);

    Packet_Guild_Guild_Cargo_Move_Item pkt;
    pkt.m_guildKey = user->get_charac_guildkey();               // → +0x0a
    pkt.m_charNo = user->getCurCharacNo();                      // → +0x0e
    pkt.m_fromSlot = (int)req.m_field0d;                        // msg+0x0d ushort → +0x12
    pkt.m_toSlot = (int)req.m_field0f;                          // msg+0x0f ushort → +0x16
    pkt.m_count = req.m_field11;                                // msg+0x11 → +0x1a
    pkt.m_type = req.m_field15;                                 // msg+0x15 → +0x1e

    // ORIG 0x081d1a2c：size/&pkt 先于 receiver 链求值（esi/ebx 保活），无 proxy 具名槽
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket((char*)&pkt, (int)pkt.packetSize);     // 0x22

    // ORIG 0x081d1a63：history 两个参数从已写的 pkt 字段读回（-0x20/-0x1c = pkt+0x12/+0x16，P92）
    user->m_historyLog.GuildCargoMoveItem(
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xd,
        (int)pkt.m_fromSlot, (int)pkt.m_toSlot);                // ORIG 0x086867ce
    return 0;
}