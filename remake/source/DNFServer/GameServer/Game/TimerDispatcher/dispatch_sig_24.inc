int TimerWarRoomWaitCount::dispatch_sig(int p1, int p2, unsigned int p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x41, p2))
    {
        return 0;
    }
    room->ChangeState(WARROOM_STATE_2);
    return 1;
