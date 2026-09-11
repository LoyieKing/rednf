int Inter_MonitorNoticeGuildSecede::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化入 -0x14(%ebp)
    Inter_MonitorNoticeGuildSecedePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildSecedePktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号不符 → 日志
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2348,
            "Inter_MonitorNoticeGuildSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x3b);
    packet.put_int(pPkt->m_12);
    packet.put_int(pPkt->m_1a);
    packet.put_byte(                                 // msg+0x1e ushort（movzwl 零扩展）
        static_cast<int>(pPkt->m_1e));
    const int secedeNameLen =                        // msg+0x20 退会角色名
        static_cast<int>(strlen(pPkt->m_20));
    packet.put_int(secedeNameLen);
    packet.put_str(pPkt->m_20, secedeNameLen);
    const int guildNameLen =                         // msg+0x37 公会名
        static_cast<int>(strlen(pPkt->m_37));
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_37, guildNameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}