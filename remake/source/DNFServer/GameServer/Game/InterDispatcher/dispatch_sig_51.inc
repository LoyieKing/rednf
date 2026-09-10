int Inter_MonitorNoticeGuildCreate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0x267c;                                 // 门失败返回 0x267c
    // ORIG：包基址槽在守卫之后物化。
    Inter_MonitorNoticeGuildCreatePktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildCreatePktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_e)
    {   // m_uCharID 不匹配 → 日志后返回 0
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2682,
            "Packet_Monitor_Notice_Guild_Create_ToUser::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    user->set_charac_guildkey(pPkt->m_a);

    PacketGuard packet;
    packet.put_header(0, 0x47);
    packet.put_int(pPkt->m_a);
    const int nameLen = strlen(pPkt->m_12);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_12, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
