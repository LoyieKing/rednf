bool TimerCheckPowerWarAbuser::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        G_GameWorld()->OnBanAbuserFromPowerWar();
        registNextTimer(0x3c);
    }
    return 1;
}