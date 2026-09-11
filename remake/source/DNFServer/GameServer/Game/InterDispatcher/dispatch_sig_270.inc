// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084e4b02）：混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本波已归零；AE 归一化后逐条相等）
int Inter_PunishCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PunishCancelPktView* pPkt = reinterpret_cast<Inter_PunishCancelPktView*>(data);
    if (user != 0 &&
        pPkt->m_e == 2 &&
        pPkt->m_10 == 0x20 &&
        user->isTradePunishType(0x20))
    {
        user->DelTradePunishType(
            (int)(unsigned short)pPkt->m_10);

        PacketGuard packet;
        packet.put_header(0, 0x13f);
        packet.put_short((unsigned short)pPkt->m_e);
        packet.put_short((unsigned short)pPkt->m_10);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}