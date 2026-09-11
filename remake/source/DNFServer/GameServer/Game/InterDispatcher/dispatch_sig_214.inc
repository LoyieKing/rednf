int Inter_TakeScreenShot::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TakeScreenShotPktView* pPkt = reinterpret_cast<Inter_TakeScreenShotPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0xf4);
    packet.put_int(pPkt->m_b);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    return 0;
}