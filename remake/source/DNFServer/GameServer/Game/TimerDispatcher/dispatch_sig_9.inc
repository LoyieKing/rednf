bool TimerPartyWalkOutLackUser::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08633e7a：GetParty(p1) 按定时器 party index 取队伍（非无参重载）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xf, p2))
    {
        return 0;
    }
    if (!party->OnTimerWalkOutLackUser())
    {
        return 0;
    }
    return 1;
}