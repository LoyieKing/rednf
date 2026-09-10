int TimerPVPWalkOutLackUser::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08633e86：GetPvp(p1, 0, 0x31)；timer_key 与 p2 比较（非 p3）。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 0x31);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x31) != p2)
    {
        return 0;
    }
    if (!room->OnTimerWalkOutLackUser())
    {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
    return 1;
