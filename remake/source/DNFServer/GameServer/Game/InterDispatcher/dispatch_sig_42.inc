int Inter_MonitorCallGuildMembersNext::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0xc(%ebp)
    Inter_MonitorCallGuildMembersNextPktView* pPkt = reinterpret_cast<Inter_MonitorCallGuildMembersNextPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x24e7,
            "Inter_MonitorCallGuildMembersNext::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (user->get_charac_guildkey() !=
        pPkt->m_12)        // msg+0x12 公会 key 校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x24ed,
            "Inter_MonitorCallGuildMembersNext::dispatch_sig() : pUser->get_charac_guildkey() != recv->m_uGuildKey");
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x8e);
    packet.put_byte(1);
    packet.put_int(pPkt->m_12);
    FunctionGuildMember::MakePacketGuildMembersInfo( // ORIG 0x084ca37a T
        packet,
        static_cast<int>(pPkt->m_16),
        reinterpret_cast<ST_Guild_Mem_Info*>(
            reinterpret_cast<char*>(pPkt) + 0x17));   // ORIG 经 -0xc 槽
    packet.finalize(true);
    user->Send(packet);
    return 0;
}