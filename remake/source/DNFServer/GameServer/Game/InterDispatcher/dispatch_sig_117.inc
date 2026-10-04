int Inter_NoticeGuildMasterDelegate::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_NoticeGuildMasterDelegatePktView* pPkt = reinterpret_cast<Inter_NoticeGuildMasterDelegatePktView*>(data);


    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {   // m_uCharID 不匹配 → 日志后退出
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x23a0,
            "Inter_NoticeGuildMasterDelegate::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_characNo);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x95);
    const int nameLen = strlen(pPkt->m_name);
    packet.put_int(nameLen);
    packet.put_str(pPkt->m_name, nameLen);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}