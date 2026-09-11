bool Timer_RoutingItemWait::dispatch_sig(int p1, int p2, memberIdentificationNumber_t /*p3*/)
{
    // ORIG 0x086398a8：null 早退（jne 岛）+ `!check` 早退（xor$1;test;je，
    // 0x086398c9）；mgr 具名槽 -0xc（party=-0x10 后使用，0x086398d8 mov %eax,-0xc）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2b, p2))
        return 0;
    CSpecialItemRoutingManager* mgr = G_CGameManager()->GetSpecialItemRoutingManager();
    mgr->ProcessTimeOut(party);
    // GetSpecialItemRoutingManager 返回值域：CGameManager.h 仅前向，见 TimerBatch.md
    return 1;
}