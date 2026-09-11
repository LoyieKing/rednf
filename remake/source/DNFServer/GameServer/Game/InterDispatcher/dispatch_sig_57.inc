int Inter_MonitorNoticeMemberLoginOut::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x27b5,
            "MEMBER : Inter_MonitorNoticeMemberLoginOut::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    // ORIG：包基址在两道 guard 之后物化入 -0x10(%ebp)
    Inter_MonitorNoticeMemberLoginOutPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberLoginOutPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_f)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x27bc,
            "MEMBER : Inter_MonitorNoticeMemberLoginOut::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_f);
        return 0;
    }

    if (pPkt->m_a == '\x01' &&
        pPkt->m_14 == '\x01')
    {
        user->set_connect_upper_member(
            pPkt->m_34);
        user->set_uppermember_exp_level(
            pPkt->m_33);
    }
    else if (pPkt->m_a == '\0' &&
             pPkt->m_14 == '\x01')
    {
        user->set_connect_upper_member(0);
        user->set_uppermember_exp_level('\0');
    }

    PacketGuard packet;
    packet.put_header(0, 0x48);
    packet.put_byte(
        (unsigned int)pPkt->m_a);
    packet.put_byte(
        (unsigned int)pPkt->m_13);
    packet.put_byte(
        (unsigned int)pPkt->m_14);
    // len 槽 int 精确匹配 put_str(char*,int)（size_t 致 decay 参数降级 add+反序）
    const int local_10 = (int)strlen(pPkt->m_15);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_15, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}