int Inter_SendTimeSync::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_SendTimeSyncPktView* pPkt = reinterpret_cast<Inter_SendTimeSyncPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x12c);
    packet.put_short((short)pPkt->m_server);
    packet.put_short((short)pPkt->m_channel);
    packet.finalize(true);
    G_GameWorld()->send_all_with_state(packet, 3);
    return 0;
}