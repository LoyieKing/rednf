int Inter_Monitor_MegaphoneMsgHyperLink::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Monitor_MegaphoneMsgHyperLinkPktView* pPkt = reinterpret_cast<Inter_Monitor_MegaphoneMsgHyperLinkPktView*>(data);
    PacketGuard packet;
    packet.put_header(0, 0x174);
    packet.put_byte(
        (int)pPkt->m_e);
    packet.put_byte(
        (int)pPkt->m_b);
    packet.put_short(
        (int)(unsigned short)pPkt->m_c);
    // ORIG 0x84e5cae/0x84e5ccb：strlen 双调（put_int/put_str 各算一次，O0 不 CSE）
    packet.put_int((int)strlen(pPkt->m_f));
    packet.put_str(pPkt->m_f, (int)strlen(pPkt->m_f));
    const unsigned int linkLen =
        (unsigned int)pPkt->m_166;
    packet.put_int((int)linkLen);
    packet.put_str(pPkt->m_167, (int)linkLen);
    packet.put_byte(
        (int)pPkt->m_2d);
    for (int i = 0;
         i < (int)(unsigned int)pPkt->m_2d;
         ++i)
    {
        packet.put_binary(data + i * 0x68 + 0x2e, 0x68);
    }
    packet.finalize(true);

    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        G_GameWorld()->send_all(packet);
    }
    else
    {
        const char group =
            pPkt->m_a;
        G_GameWorld()->send_server_group(packet, group);
    }
    return 0;
