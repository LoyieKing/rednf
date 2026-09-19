int PvP_Room::HandleTimerUserRevive(int seat)
{
    // ORIG 085dd924/085dd927/085dd92a/085dd92d：四 skip 条件各落独立 nop 岛
    // （js/jg/je/jne→nop×4，末岛 fall merge）；P_WIN 非零跳形 = if(P_WIN) 直写。
    if (seat < 0)
        goto HTR_OUT_RANGE1;
    if (seat > 7)
        goto HTR_OUT_RANGE2;
    if (P_SEATS(seat) == 0)
        goto HTR_OUT_SEAT;
    if (P_WIN(seat))
        goto HTR_OUT_WIN;
    P_WIN(seat) = 1;
    {
        PacketGuard local_18;
        local_18.put_header(0, 0x78);
        local_18.put_byte(seat);
        local_18.put_byte(rand() % 0x100);
        local_18.finalize(true);
        send_to_pvp(local_18);
    }
HTR_OUT_WIN:
    ;
HTR_OUT_SEAT:
    ;
HTR_OUT_RANGE2:
    ;
HTR_OUT_RANGE1:
    ;
#if !remake_verify
    return 0;   // DNF-FIX: 非 void 函数落底 —— g++ 13 在此发 ud2（ORIG 的 GCC 4.x 只是返回残留 eax）
#endif
}