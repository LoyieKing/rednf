// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::set_recv_pvp_end_result_flag（原 TU 第 957 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::set_recv_pvp_end_result_flag(CUser* user)
{
    // ORIG 0x085dbfec：seat<0 早退 mov $0（fall 侧），显式 if/return 物化，
    // 非 bool 算术化（避免 not/shr 折叠）。
    int seat = get_user_seat(user);
    if (seat < 0)
    {
        return 0;
    }
    m_recvFlagBC.SetRecvFlag(seat);
    return 1;
}
