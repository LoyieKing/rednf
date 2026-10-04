// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084ce5f4）：混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本波已归零；AE 归一化后逐条相等）
int Inter_TriggerEventStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TriggerEventStartPktView* pPkt = reinterpret_cast<Inter_TriggerEventStartPktView*>(data);
    if (GlobalData::s_event_manager != 0)
    {
        if (GlobalData::s_event_manager->TriggerEventStart(
                pPkt->m_eventCode,
                pPkt->m_wp))
            return 0;
        return 0x2a8b;
    }
    return 0;
}