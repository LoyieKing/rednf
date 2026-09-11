// [b28d-lane2] verdict=DIFF ndiff=82（ORIG 0x084e3f0e）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本轮修：删具名 errCode 局部，条件与
// put_byte 实参内联 pPkt->m_a ⇒ ORIG 的逐次重读形 + 帧归位）。
int Inter_ApproveJoinGuild::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ApproveJoinGuildPktView* pPkt = reinterpret_cast<Inter_ApproveJoinGuildPktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    register int charIdMsg =                           // recv charId @ msg+0xe
        pPkt->m_e;
    if (charIdMsg != user->getCurCharacNo())
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x15f);
    if (pPkt->m_a != 0)
    {
        packet.put_byte(0);
        packet.put_byte((int)pPkt->m_a);
        packet.put_int(pPkt->m_16);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int(pPkt->m_16);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}