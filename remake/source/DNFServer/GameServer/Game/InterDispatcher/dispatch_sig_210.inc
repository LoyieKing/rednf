int Inter_GuildCargoMoveItem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_GuildCargoMoveItemPktView* pPkt = reinterpret_cast<Inter_GuildCargoMoveItemPktView*>(data);
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    if (user->get_charac_no(-1) != pPkt->m_e)
        return 0;

    user->m_historyLog.GuildCargoMoveItemResult(       // +0x79700
        (ENUM_GUILD_CARGO_HISTORY_REASON)0xe,
        pPkt->m_12,
        pPkt->m_13,
        pPkt->m_17);

    PacketGuard packet;
    packet.put_header(1, 0xfd);
    // ORIG 0x084dec77 `cmp $0xc1,%al; je then-island`：false-path（else 体）在前
    // 作 fall-through（技巧17），then 为 taken 岛。
    if (pPkt->m_12 != 0xc1)
    {
        packet.put_byte(0);
        packet.put_byte((unsigned int)pPkt->m_12);
    }
    else
    {   // 结果码 0xc1 → 成功标记
        packet.put_byte(1);
    }
    packet.put_short(pPkt->m_13);
    packet.put_short(pPkt->m_17);
    packet.finalize(true);
    user->Send(packet);
    return 0;
