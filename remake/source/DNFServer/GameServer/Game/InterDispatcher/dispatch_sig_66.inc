int Inter_MonitorCallMemberList::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084e****：state 岛后 pPkt 才物化 -0x20（技巧13）。
    Inter_MonitorCallMemberListPktView* pPkt = reinterpret_cast<Inter_MonitorCallMemberListPktView*>(data);
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

    unsigned char* entry = &pPkt->m_12;   // §4.3 字段化：视图 m_12(uchar) 取址，ORIG lea 0x12(%eax)
    Inter_MonitorCallMemberListEntryWinView* pWin_entry = reinterpret_cast<Inter_MonitorCallMemberListEntryWinView*>(entry);
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
        packet.put_byte((int)pWin_entry->m_members[i].m_flags0);
        packet.put_byte((int)pWin_entry->m_members[i].m_flags1);
        const int memberNameLen = strlen(pWin_entry->m_members[i].m_name);
        packet.put_int(memberNameLen);
        packet.put_str(pWin_entry->m_members[i].m_name, memberNameLen);
        packet.put_byte((int)(char)pWin_entry->m_members[i].m_grade20);
        packet.put_byte((int)pWin_entry->m_members[i].m_grade21);
        packet.put_int(pWin_entry->m_members[i].m_22);
        packet.put_int(pWin_entry->m_members[i].m_26);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
