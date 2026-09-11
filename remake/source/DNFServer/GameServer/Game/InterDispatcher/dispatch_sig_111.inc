// [b28d-lane2] verdict=DIFF ndiff=127（ORIG 0x084d2f6e）：寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=19（ORIG 0x84d2f6e）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_ReplyGuildInviteToCaller::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // [G2A-PC 本轮负结论 N，ndiff=7] 全部差异集中在本函数首个字段校验一处：
    //   ORIG `mov %eax,%edx; mov <S>,%eax; mov 0xa(%eax),%eax; cmp %eax,%edx`
    //   （先 call get_charac_no → %edx，再读 pPkt+0xa → %eax，cmp %eax,%edx）
    //   vs 本方 `mov <S>,%edx; mov 0xa(%edx),%edx; cmp %edx,%eax`
    //   （charac_no 留在 %eax，pPkt 走 %edx）。两侧求值次序一致（charac_no 在前），
    //   仅 gcc 伪寄存器分派不同（%eax↔%edx）⇒ 无源码级单点直证，不可修，留 N。
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
}