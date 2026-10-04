int Inter_ReplyPowerWarScore::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_ReplyPowerWarScorePktView* pPkt = reinterpret_cast<Inter_ReplyPowerWarScorePktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x9b);
    packet.put_int(pPkt->m_point);
    packet.put_int(pPkt->m_channelPoint);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    GlobalData::s_power_manager->SetPowerWarPoint(
        pPkt->m_point,
        pPkt->m_channelPoint);
    return 0;
}