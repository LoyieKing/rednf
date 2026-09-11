bool TimerEPLPCommandWait::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x15, p2))
    {
        return 0;
    }
    if (party->GetSelectedEPLPCmd() == 3)
    {
        return 0;
    }
    if (party->GetEPLPState() == 2)
    {
        party->ReturnToVillage();
    }
    return 1;
}