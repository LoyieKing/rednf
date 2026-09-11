bool TimerEndPowerWarVictoriousReward::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    GlobalData::s_event_manager->TriggerEventEnd(0x35);
    return 1;
}