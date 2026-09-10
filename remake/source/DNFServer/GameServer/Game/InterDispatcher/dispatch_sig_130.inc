int Inter_ReplyPowerWarScore::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ReplyPowerWarScorePktView* pPkt = reinterpret_cast<Inter_ReplyPowerWarScorePktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x9b);
    packet.put_int(pPkt->m_a);
    packet.put_int(pPkt->m_e);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    GlobalData::s_power_manager->SetPowerWarPoint(
        pPkt->m_a,
        pPkt->m_e);
    return 0;
