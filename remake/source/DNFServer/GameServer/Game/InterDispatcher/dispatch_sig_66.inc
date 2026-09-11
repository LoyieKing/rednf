int Inter_MonitorCallMemberList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084e****：state 岛后 pPkt 才物化 -0x20（技巧13）。
    Inter_MonitorCallMemberListPktViewI* pPkt = reinterpret_cast<Inter_MonitorCallMemberListPktViewI*>(data);
    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x298c,
            "MEMBER : Inter_MonitorCallMemberList::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    if (user->get_charac_no(-1) != pPkt->m_charID)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2993,
            "MEMBER : Inter_MonitorCallMemberList::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_charID);
        return 0;
    }

    // ORIG 084d0fxx：`mov <pPkt 槽>,%eax; add $0x12,%eax; mov %eax,<槽>`（单槽一次 store）
    //   ⇒ 原码只有一个具名指针（P0：无 `entry` 中间量）。
    Inter_MonitorCallMemberListEntryWinView* pWin_entry =
        reinterpret_cast<Inter_MonitorCallMemberListEntryWinView*>(&pPkt->m_12);
    // [K3 2026-09-11] P0/P183 具名成员数组基址：ORIG 084d0fxx 段循环里发
    //   `mov -0x10(%ebp),%eax; imul $0x2a,%eax,%eax; add -0x14(%ebp),%eax; add $0x2,%eax`
    //   （成员基址只存一次槽 -0x14 = pWin_entry+0x2b；逐处只有 1 条 add，帧 0x50）。
    //   我方原把 `pWin_entry->m_members[i]` 逐处展开 ⇒ 每处多 1 条 `add $0x20`（帧 0x40，+11 条）。
    Inter_MonitorCallMemberListMemberWinView* pMember = pWin_entry->m_members;
    PacketGuard packet;
    packet.put_header(1, 0x52);
    packet.put_byte(1);
    packet.put_byte((int)pWin_entry->m_flags0);
    packet.put_byte((int)pWin_entry->m_flags1);
    const int nameLen = strlen(pWin_entry->m_name);
    packet.put_int(nameLen);
    packet.put_str(pWin_entry->m_name, nameLen);
    packet.put_byte((int)(char)pWin_entry->m_grade20);
    packet.put_byte((int)pWin_entry->m_grade21);
    packet.put_int(pWin_entry->m_22);
    packet.put_int(pWin_entry->m_26);

    packet.put_byte((int)pWin_entry->m_memberCnt);
    for (int i = 0; i < (int)pWin_entry->m_memberCnt; ++i)
    {
        packet.put_byte((int)pMember[i].m_flags0);
        packet.put_byte((int)pMember[i].m_flags1);
        const int memberNameLen = strlen(pMember[i].m_name);
        packet.put_int(memberNameLen);
        packet.put_str(pMember[i].m_name, memberNameLen);
        packet.put_byte((int)(char)pMember[i].m_grade20);
        packet.put_byte((int)pMember[i].m_grade21);
        packet.put_int(pMember[i].m_22);
        packet.put_int(pMember[i].m_26);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}