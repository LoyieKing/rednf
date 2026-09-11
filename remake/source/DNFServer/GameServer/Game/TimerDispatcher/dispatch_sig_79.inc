// ORIG T 0x0863703a (_ZN26TimerCheckRecvPvpEndResult12dispatch_sigEiij)
bool TimerCheckRecvPvpEndResult::dispatch_sig(int p1, int p2, memberIdentificationNumber_t p3)
{
    // ORIG 0x0863703a：GetPvp 首参=0xc(%ebp)=p1；timer key 比较=0x10(%ebp)=p2
    // （旧版误用 p2/p3，Ghidra 参数标注偏移不可信，以 asm 为准）。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, NULL, 0x3d);
    if (room == NULL)
        return 0;
    if (room->get_timer_key((TIMER_MESSAGE)0x3d) != p2)
        return 0;
    if (room->get_recv_pvp_end_result_count() != 0)
    {
        room->end_pvp_result();
        PacketGuard packet;
        room->make_state_info((char*)&packet);
        G_GameWorld()->send_all(packet);
        room->end_pvp();
        room->battle_reset();
    }
    return 1;
}