// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::recv_pvp_rank_all（原 TU 第 871 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N95] ORIG 0x085dc4ba 槽基 -0x4（帧 $0x18）vs 4.4 -0xc（帧 $0x28）：4.4 局部槽基恒留 8B gap 不可上移（N95 判例），指令/语义全等、P167 CLEAR。
// [b28d-solo-PvP_Room] AE: P210② IsRecv 定义迁入本 TU 并前置（Main 指派）⇒ 4.4
// 精确继承小帧，ORIG 0x085dc4ba 帧 $0x18/槽 -0x4 归位；原 N95 槽基差随定义可见性消除。
int PvP_Room::recv_pvp_rank_all() const
{
    for (int local_8 = 0; local_8 <= 7; ++local_8)
    {
        if (P_SEATS(local_8) == 0)
            continue;
        if (P_TEAMS(local_8) == 0xff)
            continue;
        if (m_recvFlagB4.IsRecv(local_8) == false)
            return 0;
    }
    return 1;
}
