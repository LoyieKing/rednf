void CQuickPartySystemManager::insert_timer_waiting_start_game(CParty* party)
{
    // ORIG：gen_timer_key→%esi、GetPartyIndex→%ebx callee-saved 缓存（P24 内联实参）
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1,
                                party->GetPartyIndex(),
                                (TIMER_MESSAGE)0x2d, 0x5a,
                                party->gen_timer_key((TIMER_MESSAGE)0x2d),
                                static_cast<TIMER_MESSAGE>(0x2d));
