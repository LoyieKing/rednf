bool TimerWarRoomBattlePauseCount::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08634df6：GetWarRoom(p1) + CheckTimerKey(0x43, p2)。
    // 拆分早退（同 ReadyLockCount，ORIG 0x08634e15/0x08634e37）。
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x43, p2))
        return 0;
    room->ChangeState(WARROOM_STATE_3);
    return 1;
}