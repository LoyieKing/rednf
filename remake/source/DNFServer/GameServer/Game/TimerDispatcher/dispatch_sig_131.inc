int Timer_TOD_Send_ClearReward::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2)) {
        return 0;
    }
    if (!party->OnTODRewardTimer((TIMER_MESSAGE)p3)) {
        return 0;
    }
    return 1;
