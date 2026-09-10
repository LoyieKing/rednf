int Inter_TriggerEventStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TriggerEventStartPktView* pPkt = reinterpret_cast<Inter_TriggerEventStartPktView*>(data);
    if (GlobalData::s_event_manager != 0)
    {
        if (GlobalData::s_event_manager->TriggerEventStart(
                pPkt->m_a,
                pPkt->m_wp))
            return 0;
        return 0x2a8b;
    }
    return 0;
