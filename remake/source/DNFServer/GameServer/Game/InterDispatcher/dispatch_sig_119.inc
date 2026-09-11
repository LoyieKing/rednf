int Inter_NoticeHasBeenGuildMaster::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_NoticeHasBeenGuildMasterPktView* pPkt = reinterpret_cast<Inter_NoticeHasBeenGuildMasterPktView*>(data);

    // ORIG：同 ReplyGuildLetter（unsigned 字段失配 4 行 spill + log 独立装载）。
    if (user->get_charac_no(-1) != pPkt->m_a)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x23da,
            "Inter_NoticeHasBeenGuildMaster::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_a);   // ORIG 一步装载，同上
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x96);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}