int Timer_ConnectP2PPvPTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, 0x3b);
    if (room == NULL) {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x3b, p2)) {
        return 0;
    }
    room->OnConnectP2PPvPTimeout();
    return 1;
