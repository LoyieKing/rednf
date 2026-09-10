int Inter_MonitorNoticeGuildChatMsg::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 该件包基址槽在 state 门之后物化（同 Inter_Monitor_DeleteBlackListResult
    // 配方：`mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` 落于守卫之后）⇒ 声明下移。
    Inter_MonitorNoticeGuildChatMsgPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildChatMsgPktView*>(data);

    if (user->get_charac_no(-1) !=
        pPkt->m_e)         // msg+0x0e 角色号校验
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x23f1,
            "Inter_MonitorNoticeGuildChatMsg::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x41);
    packet.put_byte(6);
    packet.put_byte(0);
    const int msgLen =                               // msg+0x12 聊天文本
        static_cast<int>(strlen(pPkt->m_12));
    packet.put_int(msgLen);
    packet.put_str(pPkt->m_12, msgLen);
    packet.put_byte(0);
    const unsigned int nameLen =                     // msg+0x30 发言者名长
        static_cast<unsigned int>(pPkt->m_30);
    packet.put_int(static_cast<int>(nameLen));
    packet.put_str(pPkt->m_31, static_cast<int>(nameLen));
    packet.finalize(true);
    user->Send(packet);
    return 0;
