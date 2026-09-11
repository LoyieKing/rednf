// [b28d-solo-PvP_Room] P210 定义前置：caller 置于 IsPvpObserver 定义之前 ⇒ 4.4 保守
// outgoing（帧 $0x28、槽基 -0xc），与 ORIG 0x085d65e8 逐条一致；旧 P190 pads 形帧同
// 但槽基浅 8（-0x4）已废。
bool PvP_Room::check_start_state()
{
    lock();
    // [b28d-solo-PvP_Room] ORIG 0x085d6602 前两条件落 bool 物化岛（mov$1/jmp/
    // mov$0/test/je）再与第三条件短路；for(i<=7) setle 尾测（P226+P51）。
    for (int local_10 = 0; local_10 <= 7; local_10 = local_10 + 1)
    {
        if (({ (P_SEATS(local_10) != 0) && (!IsPvpObserver(local_10)); }))
        {
            if (!P_SEATSTATE(local_10))
            {
                unlock();
                return 0;
            }
        }
    }
    unlock();
    return 1;
}