int Inter_MonitorServerEventStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorServerEventStartPktView* pPkt = reinterpret_cast<Inter_MonitorServerEventStartPktView*>(data);

    PacketGuard packet;                                // ORIG 构造后未使用，分支外统一析构

    if (pPkt->m_a == 1)
    {
        SendEventNotiPacket(true,
                            (short)pPkt->m_a,
                            (short)pPkt->m_10,
                            reinterpret_cast<CUser*>(0));
        CServerEvent().AddExpRate((int)pPkt->m_e);
        CServerEvent().TurnOnEvent(pPkt->m_a,
                                   (int)pPkt->m_10);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x53,
                                    (int)pPkt->m_10,
                                    (int)pPkt->m_e,
                                    (unsigned int)pPkt->m_a);
        cMyTrace(__PRETTY_FUNCTION__,
            0x2c48, 0)("X-Mas Event Start");
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x2c4b, 5)("Unknown ServerEvent (%d)", pPkt->m_a);
    }
    return 0;
