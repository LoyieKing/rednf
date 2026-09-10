int Inter_TriggerEventEnd::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TriggerEventEndPktView* pPkt = reinterpret_cast<Inter_TriggerEventEndPktView*>(data);
    // ORIG 0x084ce656：字段读入 %ebx 跨 cMyTrace ctor 缓存（block-scope register）。
    register unsigned int evCode = pPkt->m_a;
    cMyTrace(__PRETTY_FUNCTION__, 0x2a96, 0)("Inter_TriggerEventEnd::dispatch_sig\tevent code(%d), parameter(%d)",
          evCode);
    if (GlobalData::s_event_manager != 0)
    {
        if (GlobalData::s_event_manager->TriggerEventEnd(
                (int)pPkt->m_a))
            return 0;
        return 0x2a9a;
    }
    return 0;
