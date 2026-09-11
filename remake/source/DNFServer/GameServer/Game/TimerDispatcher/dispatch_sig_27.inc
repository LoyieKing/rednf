bool TimerWarRoomResultCount::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x44, p2))
    {
        return 0;
    }
    room->HandleTimerResultCount();
    return 1;
}