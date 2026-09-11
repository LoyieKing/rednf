// [b28d-7 N95] ORIG 0x085dc4ba 槽基 -0x4（帧 $0x18）vs 4.4 -0xc（帧 $0x28）：4.4 局部槽基恒留 8B gap 不可上移（N95 判例），指令/语义全等、P167 CLEAR。
// [b28d-solo-PvP_Room] AE: P210② IsRecv 定义迁入本 TU 并前置（Main 指派）⇒ 4.4
// 精确继承小帧，ORIG 0x085dc4ba 帧 $0x18/槽 -0x4 归位；原 N95 槽基差随定义可见性消除。
bool PvP_Room::recv_pvp_rank_all() const
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