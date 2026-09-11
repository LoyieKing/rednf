int Inter_NoticeChangeGuildNameToMembers::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    char* recv = data;                                 // ORIG 0x084d51e4 入口单槽 -0x10（技巧36）

    if (user->get_charac_no(-1)
        != reinterpret_cast<Inter_NoticeChangeGuildNameToMembersPktView*>(recv)->m_e)
    {   // m_uCharId 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x36ca,
            "Inter_NoticeChangeGuildNameToMembers::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharId(%d)",
            user->get_charac_no(-1),
            reinterpret_cast<Inter_NoticeChangeGuildNameToMembersPktView*>(recv)->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0xda);
    int nameLen = strlen(
        reinterpret_cast<Inter_NoticeChangeGuildNameToMembersPktView*>(recv)->m_16);
    packet.put_int(nameLen);
    packet.put_str(
        reinterpret_cast<Inter_NoticeChangeGuildNameToMembersPktView*>(recv)->m_16,
        nameLen);
    packet.finalize(true);
    user->Send(packet);
    user->ChangeGuildName(recv + 0x16);
    return 0;
}