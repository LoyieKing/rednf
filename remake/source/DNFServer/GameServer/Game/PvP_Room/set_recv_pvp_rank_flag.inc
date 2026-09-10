// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::set_recv_pvp_rank_flag（原 TU 第 942 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

bool PvP_Room::set_recv_pvp_rank_flag(CUser* user)
{
    int iVar2 = get_user_seat(user);
    if (iVar2 < 0)
    {
        return 0;
    }
    if (m_recvFlagB4.IsRecv(iVar2))
    {
        return 0;
    }
    m_recvFlagB4.SetRecvFlag(iVar2);
    return 1;
}
