int Inter_Monitor_RegisterBlackListResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    Inter_Monitor_RegisterBlackListResultPktView* pPkt = reinterpret_cast<Inter_Monitor_RegisterBlackListResultPktView*>(data);
    PacketGuard packet;
    packet.put_header(1, 0x79);

    if (pPkt->m_30 == '\x01')
    {
        if (!user->registerToBlackList(
                pPkt->m_2c))
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x2f90,
                "_BLACK_LIST_ :: false == pUser->registerToBlackList( recv->m_uCharacNo )");
            packet.put_byte(0);
            packet.put_int(0x4a);
        }
        else
        {
            packet.put_byte(1);
        }
    }
    else if (pPkt->m_30 == '\x02')
    {
        packet.put_byte(0);
        packet.put_int(0x4a);
    }
    else if (pPkt->m_30 == '\x03')
    {
        packet.put_byte(0);
        packet.put_int(0x4c);
    }
    else if (pPkt->m_30 == '\x04')
    {
        packet.put_byte(0);
        packet.put_int(0x4e);
    }
    else if (pPkt->m_30 == '\x05')
    {
        packet.put_byte(0);
        packet.put_int(0x5a);
    }

    const int local_10 = (int)strlen(pPkt->m_e);
    packet.put_int(local_10);
    packet.put_str(pPkt->m_e, local_10);
    packet.finalize(true);
    user->Send(packet);
    return 0;
