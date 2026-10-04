int Inter_MonitorNoticeMemberSecedeToSeceder::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化
    Inter_MonitorNoticeMemberSecedeToSecederPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberSecedeToSecederPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_characNo)         // msg+0x0e 退会角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2947,
            "MEMBER : Inter_MonitorNoticeMemberSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uSecededCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_characNo);
        return 0;
    }

    if (pPkt->m_flag == 1)   // msg+0x12 标志
    {
        user->set_connect_upper_member(0);
        user->set_uppermember_exp_level(0);
        CParty* party = (CParty*)user->GetParty();
        if (party != 0)
            party->send_party_realtime_info();
    }

    PacketGuard packet;
    packet.put_header(0, 0x4d);
    const int guildNameLen =                         // msg+0x13 公会名
        static_cast<int>(strlen(pPkt->m_guildName));
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_guildName, guildNameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}