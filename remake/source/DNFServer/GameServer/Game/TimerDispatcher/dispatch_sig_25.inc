bool TimerWarRoomReadyLockCount::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x08634d86：GetWarRoom(p1) + CheckTimerKey(0x42, p2)。
    // 拆分早退（ORIG 0x08634da5：cmpl;jne 前跳+mov0 岛；0x08634dc7 xor$1;test;je
    // 前跳 STATE + mov0 岛；ChangeState 后 mov$1 直落 EXIT）。
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x42, p2))
        return 0;
    room->ChangeState(WARROOM_STATE_3);
    return 1;
}