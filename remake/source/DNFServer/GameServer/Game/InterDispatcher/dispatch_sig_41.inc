int Inter_MonitorCallGuildMembers::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_MonitorCallGuildMembersPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildMembersPktView*>(data);

    // ORIG：unsigned 字段失配 4 行 spill（同 ReplyGuildLetter）。
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x24b9,
            "Inter_MonitorCallGuildMembers::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_guildKey)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x24bf,
            "Inter_MonitorCallGuildMembers::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x46);
    packet.put_byte(1);
    packet.put_int(pPkt->m_guildKey);
    // ORIG：strlen 结果 int 槽复用；put_str 的指针实参（`lea 0x16(%eax),%edx`）先求值。
    const int msgLen = (int)strlen(pPkt->m_msg);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_msg, msgLen);
    packet.put_int(pPkt->m_2e);
    packet.put_short(
        (int)(unsigned short)pPkt->m_32);
    FunctionGuildMember::MakePacketGuildMembersInfo(
        packet,
        (int)pPkt->m_2d,
        reinterpret_cast<ST_Guild_Mem_Info*>(pPkt->m_members));
    packet.finalize(true);
    user->Send(packet);
    return 0;
}