int Inter_MonitorNoticeGuildDismiss::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 0x084ca02f：包基址在 get_state 检查之后物化入 -0xc(%ebp)
    Inter_MonitorNoticeGuildDismissPktViewI* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildDismissPktViewI*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_characNo)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2429,
            "Inter_MonitorNoticeGuildDismiss::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_characNo);
        return 0;
    }

    cMyTrace(__PRETTY_FUNCTION__,
        0x242d, 0)("Guild Create Right Init! Char id : %d, Master Id : %d",
          user->get_charac_no(-1),
          static_cast<STGuildDBInfoOnly*>(user->GetGuildDBInfo())->m_masterNo);

    // 会长本人额外清创建权（GmCmds_C.cpp 同型先例）
    if (static_cast<STGuildDBInfoOnly*>(user->GetGuildDBInfo())->m_masterNo ==
        user->get_charac_no(-1))
    {
        user->set_guild_create_right(0);
    }
    user->set_charac_guildkey(0);

    PacketGuard packet;
    packet.put_header(0, 0x3d);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}