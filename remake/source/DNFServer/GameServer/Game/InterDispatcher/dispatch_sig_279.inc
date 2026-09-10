int Inter_MonitorNoticeGuildChatMsgHyperLink::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_MonitorNoticeGuildChatMsgHyperLinkPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildChatMsgHyperLinkPktView*>(data);
    // §P0：ORIG 084e5080/084e508f 两次独立重读 msg+0xe（比较用 eax、logFormat 用 ebx 跨 call 保活，
    //   均无槽往返）⇒ 不设具名 characNo。
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x63e4,
            "Inter_MonitorNoticeGuildChatMsgHyperLink::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x173);
    packet.put_byte(6);
    packet.put_byte(0);
    const int msgLen = (int)strlen(pPkt->m_12);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_12, msgLen);
    packet.put_byte(0);
    // §P0：ORIG 084e5184/084e51a0 两次独立重读 msg+0x169（put_int 与 put_str 各自 movzbl 双段，无槽）。
    packet.put_int((int)(unsigned int)pPkt->m_169);
    packet.put_str(pPkt->m_16a, (int)(unsigned int)pPkt->m_169);
    packet.put_byte(
        (int)pPkt->m_30);
    for (int i = 0;
         i < (int)(unsigned int)pPkt->m_30;
         ++i)
    {
        packet.put_binary(pPkt->m_items[i].m_0, 0x68);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
