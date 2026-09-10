int Inter_ReplyGuildInviteToCaller::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_ReplyGuildInviteToCallerPktView* pPkt = reinterpret_cast<Inter_ReplyGuildInviteToCallerPktView*>(data);
    // ORIG 0x084d2f**：m_a/m_12 各处独立 reload（P48 无 CSE，无具名槽）。
    if (user->get_charac_no(-1) != pPkt->m_a)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x33cf,
            "MEMBER : Inter_ReplyGuildInviteToCaller::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_a);
        return 0;
    }

    if (pPkt->m_12 == 2)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            reinterpret_cast<char*>(pPkt) + 0x16,   // ORIG 0x084d2f8x 从 pPkt 槽 -0x10 取址
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(1),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(4));
        return 0;
    }
    if (pPkt->m_12 == 3)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            reinterpret_cast<char*>(pPkt) + 0x16,   // 同上（P0：ORIG 有具名槽往返）
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(0),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(4));
        return 0;
    }

    PacketGuard packet;
    packet.put_header(0, 0x94);
    packet.put_byte(pPkt->m_12);
    // ORIG ja：unsigned 比较（signed jg 形不符）。
    if ((unsigned int)pPkt->m_12 < 2u)
    {
        const int msgLen = (int)strlen(pPkt->m_16);
        packet.put_int(msgLen);
        packet.put_str(pPkt->m_16, msgLen);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
