int Inter_NoticePowerWarProlong::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_NoticePowerWarProlongPktView* pPkt = reinterpret_cast<Inter_NoticePowerWarProlongPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0xa3);
    packet.put_int(pPkt->m_sec);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}