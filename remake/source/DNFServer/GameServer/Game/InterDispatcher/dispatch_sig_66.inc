#pragma pack(push, 1)
// P248 同型化：ORIG 084d0fxx 比较处发 4 行
//   `mov %eax,%edx; mov -0x20(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ +0x0e 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证）。
//   聚合器 Inter_MonitorCallMemberListPktView.m_charID 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorCallMemberListPktViewI
{
    char m_pad0[0xe];               // +0x0..未消费
    unsigned int m_charID;          // +0x0e
    unsigned char m_12;             // +0x12
};
#pragma pack(pop)

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
