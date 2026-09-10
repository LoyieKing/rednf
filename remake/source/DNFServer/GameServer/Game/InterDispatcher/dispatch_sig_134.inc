int Inter_NoticePowerWarEndTime::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarEndTimePktView* pPkt = reinterpret_cast<Inter_NoticePowerWarEndTimePktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0xa2);
    packet.put_byte(pPkt->m_a);
    packet.put_int(pPkt->m_b);
    packet.put_int(pPkt->m_f);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
