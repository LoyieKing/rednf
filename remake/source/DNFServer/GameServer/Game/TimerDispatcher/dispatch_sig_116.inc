bool TimerStartExpEventAfterVillageAttack::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // TriggerEventStart 为 CEventManager 成员；Word_Param 低 ushort=100（ORIG movw $0x64）。
    Word_Param param;
    param.m_duration = 100;
    GlobalData::s_event_manager->TriggerEventStart(3, param);
    return 1;
}