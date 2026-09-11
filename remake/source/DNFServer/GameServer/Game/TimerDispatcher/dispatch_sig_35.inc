bool TimerBloodPhaseRegister3::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1a, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[3];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[3];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1a, 3);
    return 1;
}