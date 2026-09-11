int TimerPvpRankRes::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 0x32);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x32) != p2)
    {
        return 0;
    }
    room->pvp_reward();
    return 1;
}