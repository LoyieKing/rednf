int Inter_MonitorNoticeGuildLoginOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete → 门
        return 0;
    // ORIG：包基址槽在 get_state 守卫之后才物化（声明位置决定落栈时机）。
    Inter_MonitorNoticeGuildLoginOutPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildLoginOutPktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_f)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x232c,
            "Inter_MonitorNoticeGuildLoginOut::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_f);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x40);
    packet.put_byte((int)pPkt->m_a);   // movzbl+movsbl
    packet.put_byte((unsigned int)pPkt->m_13);
    const int nameLen = strlen(pPkt->m_14);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_14, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
