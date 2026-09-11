// ORIG 0x0860b4c2 _ZN26CSpecialItemRoutingManager11cancelTimerEP6CParty
int CSpecialItemRoutingManager::cancelTimer(CParty* party)
{
    return party->gen_timer_key((TIMER_MESSAGE)0x2b);
}