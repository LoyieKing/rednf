int Inter_MonitorGuildWarInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    if (G_GameWorld()->GetChannelType() != 6)
        return 0;

    Inter_MonitorGuildWarInfoPktView* pPkt = reinterpret_cast<Inter_MonitorGuildWarInfoPktView*>(data);
    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2ad4,
            "GuildWarInfo : Inter_MonitorGuildWarInfo::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x70);
    packet.put_byte(1);

    int local_1c = 0;
    for (int local_18 = 0; local_18 < 10; ++local_18)
    {
        if (pPkt->m_items[local_18].m_12 == 0)
            goto count_done;
        local_1c = local_1c + 1;
    count_done:;
    }
    packet.put_byte(local_1c);

    for (int local_14 = 0; local_14 < local_1c; ++local_14)
    {
        packet.put_int(pPkt->m_items[local_14].m_12);
        packet.put_int(pPkt->m_items[local_14].m_16);
        packet.put_short((unsigned int)pPkt->m_items[local_14].m_1a);
        const int local_10 = (int)strlen(pPkt->m_items[local_14].m_1c);
        packet.put_int(local_10);
        packet.put_str(pPkt->m_items[local_14].m_1c, local_10);
    }

    packet.finalize(true);
    user->Send(packet);
    return 0;
