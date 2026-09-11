bool TimerBossDieTimeout::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    // ORIG 0x08636426：直接透传 OnTimeoutBossDie 返回值（CParty.h 声明 int）。
    return party->OnTimeoutBossDie(p2, p3);
}