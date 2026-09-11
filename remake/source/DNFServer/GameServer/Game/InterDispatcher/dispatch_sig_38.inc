int Inter_MonitorNoticeGuildMarkChange::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084ca1xx：包基址在 state 门之后物化（`jmp` 之后的
    //   `mov 0x10(%ebp),%eax; mov %eax,-0x1c(%ebp)`），声明位置照抄（P0）。
    Inter_MonitorNoticeGuildMarkChangePktViewI* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildMarkChangePktViewI*>(data);


    if (user->get_charac_no(-1) != pPkt->m_e)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x244a,
            "Inter_MonitorNoticeGuildMarkChange::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    cMyTrace(__PRETTY_FUNCTION__,
        0x244f, 0)("[GUILD MARK CHANGE] Send to client. (channel:%d, character:%u, guildkey:%u)",
          pPkt->m_a,
          pPkt->m_e,
          pPkt->m_12);

    PacketGuard packet;
    packet.put_header(0, 0x3c);
    packet.put_int(pPkt->m_12);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}