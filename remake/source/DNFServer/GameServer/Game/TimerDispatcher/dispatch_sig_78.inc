bool TimerCheckPvPRoomUser::dispatch_sig(int p1, int p2, memberIdentificationNumber_t /*p3*/)
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, (int)0x3c);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x3c) != p2)
    {
        return 0;
    }
    if (room->get_state() != 2)
    {
        return 0;
    }
    // ORIG +0x80: check_recv_pvp_rank()
    room->check_recv_pvp_rank();
    // ORIG +0x90: !IsEndPvpBattle()（bool 声明 ⇒ xor $0x1,%eax；PvPR 同步 .h/.cpp）
    if (!room->IsEndPvpBattle())
    {
        room->send_pvp_end();
    }
    return 1;
}