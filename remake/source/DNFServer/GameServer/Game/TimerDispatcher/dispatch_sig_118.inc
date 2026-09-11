bool Timer_Midnight::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    G_GameWorld()->ResetMidnight();
    GlobalData::s_onlinePreliminaryTeamMgr->ResetDailyMidnight();
    const bool registered = RegistNextTimer();
    if (!registered) {
        return 0;
    }
    return 1;
}