// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::HandleTimerUserRevive（原 TU 第 2483 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

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
}
