bool TimerElevatorDungeon::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x28, p2)) {
        return 0;
    }
    // ORIG：CBattle_Field 嵌于 party+0xb24。
    reinterpret_cast<CBattle_Field*>(&party->m_battleField)
        ->check_elevator_timer();
    return 1;
}