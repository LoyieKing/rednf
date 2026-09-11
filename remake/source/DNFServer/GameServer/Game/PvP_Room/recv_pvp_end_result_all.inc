// [b28d-7 N95] ORIG 0x085dc07e 槽基 -0x4（帧 $0x18）vs 4.4 -0xc（帧 $0x28）：N95 槽基 gap 封顶，P167 CLEAR。
// [b28d-solo-PvP_Room] AE: P210② IsRecv 定义迁入 ⇒ ORIG 0x085dc07e 帧 $0x18/槽 -0x4 归位。
bool PvP_Room::recv_pvp_end_result_all() const
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