int Inter_MonitorNoticeDeleteMemberId::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG：包基址唯一槽 -0x10(%ebp)（get_state 检查后物化）
    Inter_MonitorNoticeDeleteMemberIdPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeDeleteMemberIdPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x296d,
            "MEMBER : Inter_MonitorNoticeDeleteMemberId::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    user->set_connect_upper_member(0);
    user->set_uppermember_exp_level(0);
    user->set_charac_memberkey(0);

    CParty* party = (CParty*)user->GetParty();
    if (party != 0)
        party->send_party_realtime_info();

    PacketGuard packet;
    packet.put_header(0, 0x4b);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}