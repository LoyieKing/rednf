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