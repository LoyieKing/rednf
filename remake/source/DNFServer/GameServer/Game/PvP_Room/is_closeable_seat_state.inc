// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::is_closeable_seat_state（原 TU 第 598 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-solo-PvP_Room] P210 定义前置：caller 置于 callee(IsPvpObserver) 定义之前 ⇒ 4.4
// 保守 outgoing（帧 $0x28、槽基 -0x14/-0x10/-0xc），与 ORIG 0x085d7743 逐条一致；
// 旧 P190 双取址 double pads 形只撑帧不改槽基（槽仍 -0xc 起）已废。
bool PvP_Room::is_closeable_seat_state()
{
    int local_18 = 0;
    int local_14 = 0;
    for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
    {
        if ((P_SEATS(local_10) != 0) && (!IsPvpObserver(local_10)))
        {
            local_18 = local_18 + 1;
        }
        if (P_TEAMS(local_10) == 0xff)
        {
            local_14 = local_14 + 1;
        }
    }
    if ((local_18 == 1) && (local_14 < 2))
    {
        return 0;
    }
    return 1;
}
