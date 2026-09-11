// [b28d-7 N95] ORIG 0x085dc1c8 槽 -0x8/-0x4（帧 $0x18）vs 4.4 -0x10/-0xc（帧 $0x28）：N95 槽基 gap + 帧量化，P167 CLEAR。
// [b28d-solo-PvP_Room] NEAR(39=39): P210② IsRecv 定义迁入 ⇒ 帧 $0x18/槽 -0x8/-0x4
// 归位；P223 continue 形复现三 je 分落岛；残余仅重定位显示差（N124）。
int PvP_Room::get_recv_pvp_end_result_count() const
{
    int local_c = 0;
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        // [b28d-solo-PvP_Room] P223 continue 形（同 get_recv_pvp_rank_count 判例）：
        // ORIG 0x085dc1c8 前两守卫落 L1(nop;jmp)/L2(nop) 岛、第三守卫直落 i++；
        // 正极性 `if (IsRecv) count++` 即产三 je 分落 + count++ 后 jmp 全等形。
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagBC.IsRecv(local_8))
            local_c = local_c + 1;
    }
    return local_c;
}