int TimerDungeonFail::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xe, p2))
    {
        return 0;
    }
    party->party_dungeon_fail();
    return 1;
