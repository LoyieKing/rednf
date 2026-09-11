int Inter_NpcLimitBuyItemChangeInfo::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NpcLimitBuyItemChangeInfoPktView* pPkt = reinterpret_cast<Inter_NpcLimitBuyItemChangeInfoPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x191);
    packet.put_int(pPkt->m_a);
    packet.put_int(pPkt->m_e);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}