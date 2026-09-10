int Inter_MonitorNoticeRequestMemberEnterToRequester::dispatch_sig(
    CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG：包基址在 get_state 检查后物化
    Inter_MonitorNoticeRequestMemberEnterToRequesterPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeRequestMemberEnterToRequesterPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x27e8,
            "MEMBER : Inter_MonitorNoticeRequestMemberEnterToRequester::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x4f);
    if (pPkt->m_12 == 0)   // msg+0x12 结果码
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(                             // movzbl 零扩展
            static_cast<int>(pPkt->m_12));
    }
    const int guildNameLen =                         // msg+0x13 公会名
        static_cast<int>(strlen(pPkt->m_13));
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_13, guildNameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
