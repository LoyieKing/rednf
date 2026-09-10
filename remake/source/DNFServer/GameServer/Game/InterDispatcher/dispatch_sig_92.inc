int Inter_DB_RequestBlackCount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    Inter_DB_RequestBlackCountPktView* pPkt = reinterpret_cast<Inter_DB_RequestBlackCountPktView*>(data);

    if (pPkt->m_4 == 0)
        user->setCurrCharacBlackCount(pPkt->m_8);
    else
        user->setCurrCharacBlackCount(0);
    user->EnableCharacInfo((CACHE_SECTION)7);

    if (pPkt->m_c != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0xec);
        packet.put_int(pPkt->m_c);
        packet.put_int(pPkt->m_8);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
