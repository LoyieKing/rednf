int Inter_MonitorNoticeRequestMemberEnterToResponser::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_MonitorNoticeRequestMemberEnterToResponserPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeRequestMemberEnterToResponserPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x280c,
            "MEMBER : Inter_MonitorNoticeRequestMemberEnterToResponser::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (user->alonePlay_Mode())
    {
        Packet_Monitor_Member_Enter_Reply reply;      // PacketHeader(0x4b9, 0x13)
        reply.m_idByChannel = user->GetUID();     // +0xa（权威头名）
        reply.m_charNo = user->getCurCharacNo();  // +0xe
        reply.m_code = 3;                         // +0x12
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
        proxy->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
        return 0;
    }

    if (user->GetGameOptionRef()->get_etc_option((ENUM_OPTION_ETC)0x1d) == 1)
    {
        Packet_Monitor_Member_Enter_Reply reply;      // PacketHeader(0x4b9, 0x13)
        reply.m_idByChannel = user->GetUID();
        reply.m_charNo = user->getCurCharacNo();
        reply.m_code = 4;
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
        proxy->SendTcpPacket(reinterpret_cast<char*>(&reply), 0x13);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x49);
    packet.put_short((int)pPkt->m_12);
    const int local_10 = (int)strlen(pPkt->m_14);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_14, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
