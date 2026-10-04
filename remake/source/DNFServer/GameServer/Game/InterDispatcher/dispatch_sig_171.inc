int Inter_ResposncePCRoomPlyerCount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在门 1 之后物化。
    Inter_ResposncePCRoomPlyerCountPktView* pPkt = reinterpret_cast<Inter_ResposncePCRoomPlyerCountPktView*>(data);

    // ORIG：同 ReplyGuildLetter。
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x41cc,
            "Inter_ResponsePCRoomPlayerCount::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);   // ORIG 一步装载，同上
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0xd9);
    packet.put_byte(1);
    packet.put_byte(pPkt->m_12);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}