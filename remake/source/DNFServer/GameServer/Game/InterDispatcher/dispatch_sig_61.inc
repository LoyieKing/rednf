int Inter_MonitorNoticeMemberEnterReply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeMemberEnterReplyPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberEnterReplyPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_11)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x28d9,
            "MEMBER : Inter_MonitorNoticeMemberEnterReply::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_11);
        return 0;
    }

    if (pPkt->m_a == '\x02')
        return 0;
    if (pPkt->m_a == '\x03' || pPkt->m_a == '\x04')
        return 0;

    if (pPkt->m_b == '\x01' ||
        pPkt->m_b == '\x02')
    {
        if (pPkt->m_b == '\x01' &&
            pPkt->m_c == '\x01')
        {
            user->set_uppermember_exp_level(
                pPkt->m_34);
            user->set_connect_upper_member(
                pPkt->m_35);
            CParty* local_10 = (CParty*)user->GetParty();
            if (local_10 != 0)
                local_10->send_party_realtime_info();
        }
        user->set_charac_memberkey(
            pPkt->m_11);
    }

    PacketGuard packet;
    packet.put_header(1, 0x50);
    packet.put_byte(1);
    packet.put_byte(
        (unsigned int)pPkt->m_b);
    packet.put_byte(
        (unsigned int)pPkt->m_15);
    const int local_14 = (int)strlen(pPkt->m_16);
    packet.put_int(local_14);
    packet.put_str(pPkt->m_16, local_14);
    packet.finalize(true);
    user->Send(packet);
    return 0;
