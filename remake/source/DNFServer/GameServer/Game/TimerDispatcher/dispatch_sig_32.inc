bool TimerBloodPhaseRegister0::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x17, p2))
        return 0;
    short nextRound =
        (party)->m_battleField.m_bloodSpawnCnt[0];
    short* spawnTime =
        (party)->m_battleField.m_bloodSpawnTime;
    (party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x17, 0);
    return 1;
}