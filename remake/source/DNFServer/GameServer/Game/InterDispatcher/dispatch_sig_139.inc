int Inter_SendAllUserInfoMiniumForGuildSystem::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_SendAllUserInfoMiniumForGuildSystemPktView* pPkt = reinterpret_cast<Inter_SendAllUserInfoMiniumForGuildSystemPktView*>(data);
    // ORIG：同 ReplyGuildLetter。
    if (user->get_charac_no(-1) != pPkt->m_characNo)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x36e3,
            "Inter_SendAllUserInfoMiniumForGuildSystem::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharId(%d)",
            user->get_charac_no(-1), pPkt->m_characNo);   // ORIG 一步装载，同上
        return 0;
    }

    PacketGuard packet;
    packet.clear();                                    // InterfacePacketBuf::clear
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, 0);          // pack 至 packet 缓冲
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);
    return 0;
}