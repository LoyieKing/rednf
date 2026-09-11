int Inter_MonitorNoticeGuildChatMsg::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                      // asm test eax,eax; sete
        return 0;

    // ORIG 该件包基址槽在 state 门之后物化（同 Inter_Monitor_DeleteBlackListResult
    // 配方：`mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)` 落于守卫之后）⇒ 声明下移。
    Inter_MonitorNoticeGuildChatMsgPktViewI* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildChatMsgPktViewI*>(data);

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
    // ORIG 084c9fxx：无 nameLen 槽（我方旧形多发 `mov %eax,<槽>; mov <槽>,%eax` 往返），
    //   且读形为 `movzbl 0x30(%eax),%eax; movzbl %al,%eax`（二次零扩展）= (unsigned char) 包裹
    //   ⇒ 原码内联实参。msg+0x30 发言者名长。
    packet.put_int((int)(unsigned char)pPkt->m_30);
    packet.put_str(pPkt->m_31, (int)(unsigned char)pPkt->m_30);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}