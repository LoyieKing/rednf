bool TimerBloodPhaseRegister1::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x18, p2))
        return 0;
    short nextRound =
        (party)->m_battleField.m_bloodSpawnCnt[1];
    short* spawnTime =
        &(party)->m_battleField.m_bloodSpawnTime[1];
    (party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x18, 1);
    return 1;
}