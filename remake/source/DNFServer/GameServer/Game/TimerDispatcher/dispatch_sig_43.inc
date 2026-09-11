bool TimerBloodDungeonClearReward::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08638cd0：GetParty(p1)；check_timer_key 以 p3 为 msg、p2 为 param。
    CParty* party = G_CGameManager()->GetParty(p1);
    // 拆分三段独立早退（ORIG 0x08638cd5 jne 岛 / 0x08638cf7 xor$1;test;je /
    // 0x08638d17 同形，je→8d25 mov$1 尾岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;
    if (!party->onBloodRewardTimer((TIMER_MESSAGE)p3))
        return 0;
    return 1;
}