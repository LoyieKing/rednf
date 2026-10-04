int Inter_MonitorNoticeMemberChatMsgHyperLink::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x640a,
            "MEMBER : Inter_MonitorNoticeMemberChatMsgHyperLink::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    // ORIG：包基址在两道 guard 后物化入 -0x14(%ebp)
    Inter_MonitorNoticeMemberChatMsgHyperLinkPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeMemberChatMsgHyperLinkPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x6411,
            "MEMBER : Inter_MonitorNoticeMemberChatMsgHyperLink::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_characNo);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x173);
    packet.put_byte(8);
    packet.put_byte(0);
    // len 槽 int 精确匹配（size_t 致 decay 降级 add+反序）
    const int local_14 = (int)strlen(pPkt->m_msg);
    packet.put_int(local_14);
    packet.put_str(pPkt->m_msg, local_14);
    packet.put_byte(0);
    packet.put_int(
        (unsigned int)pPkt->m_linkLen);
    packet.put_str(
        pPkt->m_link,        // +0x16a 链接串
        (unsigned int)pPkt->m_linkLen);
    packet.put_byte(
        (unsigned int)pPkt->m_itemCount);
    for (int local_10 = 0;
         local_10 < (int)(unsigned int)pPkt->m_itemCount;
         ++local_10)
    {
        // ORIG 0x084e5xxx：数组寻址两步位移形态（见 PktView ItemView 注释）。
        packet.put_binary(pPkt->m_items[local_10].m_0, 0x68);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}