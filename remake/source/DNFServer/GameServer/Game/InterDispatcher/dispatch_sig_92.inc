int Inter_DB_RequestBlackCount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;
    // ORIG：包基址槽在守卫之后物化。
    SIG_REQ_BLACK_COUNT* bc = reinterpret_cast<SIG_REQ_BLACK_COUNT*>(data);

    if (bc->m_count == 0)
        user->setCurrCharacBlackCount(bc->m_blackCount);
    else
        user->setCurrCharacBlackCount(0);
    user->EnableCharacInfo((CACHE_SECTION)7);

    if (*reinterpret_cast<int*>(bc->m_pad_c) != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0xec);
        packet.put_int(*reinterpret_cast<int*>(bc->m_pad_c));
        packet.put_int(bc->m_blackCount);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}