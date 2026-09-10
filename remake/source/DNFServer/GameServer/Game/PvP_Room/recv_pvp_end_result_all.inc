// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::recv_pvp_end_result_all（原 TU 第 887 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N95] ORIG 0x085dc07e 槽基 -0x4（帧 $0x18）vs 4.4 -0xc（帧 $0x28）：N95 槽基 gap 封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] AE: P210② IsRecv 定义迁入 ⇒ ORIG 0x085dc07e 帧 $0x18/槽 -0x4 归位。
int PvP_Room::recv_pvp_end_result_all() const
{
    for (int local_8 = 0; local_8 <= 7; ++local_8)
    {
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagBC.IsRecv(local_8) == false)
            return 0;
    }
    return 1;
}
