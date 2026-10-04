// [b28d-lane2] verdict=DIFF ndiff=71（ORIG 0x084e4c2a）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=45（ORIG 0x84e4c2a）：N18a 单段 regmove。
int Inter_BroadcastMsg::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_BroadcastMsgPktView* pPkt = reinterpret_cast<Inter_BroadcastMsgPktView*>(data);
    if (pPkt->m_ok == 0)
    {
        // [b28d-lane2] ORIG 0x084e4c5c 段无 msgLen 槽（`movzbl 0xe(%eax)` 直入 put_int/
        // put_str 实参，逐次重读）⇒ 删具名局部（P0：无槽往返 ⇒ 原码无该局部）。
        PacketGuard packet;
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int(pPkt->m_len);
        packet.put_str(pPkt->m_msg, pPkt->m_len);
        packet.finalize(true);

        CUserTeen teen;
        G_GameWorld()->send_group(packet, &teen);

    }
    return 0;
}