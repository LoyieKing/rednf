int CSpecialItemRoutingManager::cancelTimer(CParty* party)
{
    return party->gen_timer_key((TIMER_MESSAGE)0x2b);
