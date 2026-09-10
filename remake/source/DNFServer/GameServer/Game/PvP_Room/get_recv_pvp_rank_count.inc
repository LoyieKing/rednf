// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::get_recv_pvp_rank_count（原 TU 第 828 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N95] ORIG 0x085dc2cc 槽 -0x8/-0x4（帧 $0x18）vs 4.4 -0x10/-0xc（帧 $0x28）：N95 槽基 gap + 帧量化，P167 CLEAR。
// [b28d-solo-PvP_Room] NEAR(39=39): P210② IsRecv 定义迁入本 TU ⇒ 帧 $0x18/槽
// -0x8/-0x4 归位；P223 continue 形复现三 je 分落岛（+0x5b/+0x5e/+0x5f 偏移全同）；
// 残余仅 call 目标/跳转目标的重定位显示差（N124 工具口径）。
int PvP_Room::get_recv_pvp_rank_count()
{
    // ORIG 0x085dc2cc：bool IsRecv 直用条件（test %al/je，无 !=0 物化）；
    // 槽 -0x8=count、-0x4=i，帧 $0x18。
    int local_c = 0;
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        // [b28d-solo-PvP_Room] P223 continue 形：ORIG 0x085dc2cc 前两守卫落
        // L1(nop;jmp)/L2(nop) 岛、第三守卫（!IsRecv）直落 L3(i++)。
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagB4.IsRecv(local_8))
            local_c = local_c + 1;
    }
    return local_c;
}
