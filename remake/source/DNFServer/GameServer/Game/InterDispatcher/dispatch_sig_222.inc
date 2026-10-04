int Inter_FullLevelBroadCast::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_FullLevelBroadCastPktView* pPkt = reinterpret_cast<Inter_FullLevelBroadCastPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x56);
    packet.put_byte(6);
    packet.put_int((int)strlen(pPkt->m_name));
    packet.put_str(pPkt->m_name, (int)strlen(pPkt->m_name));
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}