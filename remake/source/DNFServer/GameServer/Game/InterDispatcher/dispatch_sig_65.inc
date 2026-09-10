int Inter_MonitorNoticeMemberChatMsg::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2852,
            "MEMBER : Inter_MonitorNoticeMemberChatMsg::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    Inter_MonitorNoticeMemberChatMsgPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberChatMsgPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2859,
            "MEMBER : Inter_MonitorNoticeMemberChatMsg::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x41);
    packet.put_byte(8);
    packet.put_byte(0);
    const int msgLen = (int)strlen(pPkt->m_12);
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_12, msgLen);
    packet.put_byte(0);
    // ORIG 084cd1xx：无 nameLen 槽（我方旧形多发 `mov %eax,<槽>; mov <槽>,%eax` 往返），
    //   且读形为 `movzbl 0x30(%eax),%eax; movzbl %al,%eax`（二次零扩展）= (unsigned char) 包裹
    //   ⇒ 原码内联实参。
    packet.put_int((int)(unsigned char)pPkt->m_30);
    packet.put_str(pPkt->m_31, (int)(unsigned char)pPkt->m_30);
    packet.finalize(true);
    user->Send(packet);
    return 0;
