int TimerBloodSmallRoundRegister::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08634fee：GetParty(p1) + check_timer_key(0x16, p2)。
    CParty* party = G_CGameManager()->GetParty(p1);
    // 拆分独立早退（ORIG 0x08634ffx：null 段 jne 前跳岛 + check 段 xor$1;test;je 岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x16, p2))
        return 0;
    party->m_battleField.BloodSpawnTimerManager(
        false, true);   // ORIG：party+0xb24 即 CBattle_Field 子对象
    return 1;
