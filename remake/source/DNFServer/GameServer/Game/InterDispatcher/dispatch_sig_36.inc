int Inter_MonitorNoticeGuildEnter::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeGuildEnterPktViewI* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildEnterPktViewI*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2305,
            "Inter_MonitorNoticeGuildEnter::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (pPkt->m_55 == user->getCurCharacNo())
    {
        // [b28d-solo-InterDispatcher] P0：ORIG 0x084c9259..0x084c92b4 中 cVar1→%ebx、
        // uVar2→%edi、uVar4→%esi 均无栈槽（内联右值）；仅 pcVar8 落 -0x3c（具名局部）。
        // [K3 2026-09-11] P212 物化点：ORIG `084c9280 add $0x20,%eax; mov %eax,-0x3c(%ebp)`
        //   出现在 m_59/m_12/m_1c 取值**之后**、get_charac_guildkey **之前**（实参右→左
        //   求值使该表达式跨调用落槽）⇒ 去具名声明，改实参内联形。
        user->m_historyLog.GuildJoin(
            user->get_charac_guildkey(), reinterpret_cast<char*>(pPkt) + 0x20,
            pPkt->m_1c, pPkt->m_12,
            pPkt->m_59 != 0 ? '\x01' : '\0');
    }

    PacketGuard packet;
    packet.put_header(0, 0x3a);
    packet.put_int(pPkt->m_14);
    packet.put_int(pPkt->m_1c);
    const int local_24 = (int)strlen(pPkt->m_20);
    packet.put_int(local_24);
    packet.put_str(pPkt->m_20, local_24);
    const int local_20 = (int)strlen(pPkt->m_37);
    packet.put_int(local_20);
    packet.put_str(pPkt->m_37, local_20);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}