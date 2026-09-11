int Inter_CallGuildInviteToCaller::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_CallGuildInviteToCallerPktView* pPkt = reinterpret_cast<Inter_CallGuildInviteToCallerPktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_a)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x335c,
            "MEMBER : Inter_CallGuildInviteToCaller::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_a);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x9a);
    if (pPkt->m_12 == 0)
    {   // 结果码为零 → 成功
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_12);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}