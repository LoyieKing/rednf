bool TimerSetResult::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xc, p2))
    {
        return 0;
    }
    party->forced_set_play_result();
    return 1;
}