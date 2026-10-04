// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084ce646）：混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本波已归零；AE 归一化后逐条相等）
int Inter_TriggerEventEnd::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_TriggerEventEndPktView* pPkt = reinterpret_cast<Inter_TriggerEventEndPktView*>(data);
    // ORIG 0x084ce656：字段读入 %ebx 跨 cMyTrace ctor 缓存（block-scope register）。
    register unsigned int evCode = pPkt->m_eventCode;
    cMyTrace(__PRETTY_FUNCTION__, 0x2a96, 0)("Inter_TriggerEventEnd::dispatch_sig\tevent code(%d), parameter(%d)",
          evCode);
    if (GlobalData::s_event_manager != 0)
    {
        if (GlobalData::s_event_manager->TriggerEventEnd(
                (int)pPkt->m_eventCode))
            return 0;
        return 0x2a9a;
    }
    return 0;
}