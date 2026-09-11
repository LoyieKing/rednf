bool TimerBloodPhaseRegister7::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1e, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[7];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[7];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1e, 7);
    return 1;
}