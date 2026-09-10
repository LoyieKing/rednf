void CSpecialItemRoutingManager::insertTimer(CParty* party)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, party->GetPartyIndex(),
                                (TIMER_MESSAGE)0x2b, 0x23,
                                party->gen_timer_key((TIMER_MESSAGE)0x2b), 0);
