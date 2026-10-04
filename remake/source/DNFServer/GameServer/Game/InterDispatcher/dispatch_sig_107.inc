int Inter_ReplyGuildLetter::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_ReplyGuildLetterPktView* pPkt = reinterpret_cast<Inter_ReplyGuildLetterPktView*>(data);

    // ORIG：比较处 4 行 spill（`mov %eax,%edx`+`mov -0xc(%ebp),%eax`+`mov 0xa(%eax),%eax`
    // +`cmp %eax,%edx`）＝unsigned 字段→int 返回失配；log 处再独立装载（`mov 0xa(%eax),%ebx`）。
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x3340,
            "MEMBER : Inter_ReplyGuildLetter::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);   // ORIG 一步 `mov 0xa(%eax),%ebx`（无 cast 免双级）
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x61);
    if (pPkt->m_result == '\0')
        packet.put_byte(1);
    else
        packet.put_byte(0);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}