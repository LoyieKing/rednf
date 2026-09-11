int Inter_WebNoticeSingle::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_WebNoticeSinglePktView* pPkt = reinterpret_cast<Inter_WebNoticeSinglePktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int((unsigned int)pPkt->m_a);
    packet.put_str(pPkt->m_b,
                   (unsigned int)pPkt->m_a);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}