int Inter_Monitor_MegaphoneMsg::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Monitor_MegaphoneMsgPktView* pPkt = reinterpret_cast<Inter_Monitor_MegaphoneMsgPktView*>(data);

    PacketGuard packet;
    packet.put_header(0, 0x76);
    packet.put_byte((unsigned int)pPkt->m_e);
    packet.put_byte((unsigned int)pPkt->m_b);
    packet.put_short((unsigned int)pPkt->m_c);
    packet.put_int(strlen(pPkt->m_f));
    packet.put_str(pPkt->m_f, strlen(pPkt->m_f));    // ORIG 两次独立 strlen（P48）
    packet.put_int((unsigned int)pPkt->m_2d);
    packet.put_str(pPkt->m_2e,
                   (unsigned int)pPkt->m_2d);
    packet.finalize(true);

    // ORIG 0x084d0ac9：bool 直测；非 0 → send_server_group(group=recv+0xa)，0 → send_all
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        G_GameWorld()->send_server_group(packet, pPkt->m_a);
    }
    else
    {
        G_GameWorld()->send_all(packet);
    }
    return 0;
