// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::select_new_manager（原 TU 第 2267 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::select_new_manager()
{
    for (int local_8 = 0; local_8 <= 7; local_8 = local_8 + 1)
    {
        if (P_SEATS(local_8) != 0)
        {
            if (P_SEATS(local_8) != m_seats[7])
            {
                m_seats[7] = P_SEATS(local_8);
                m_managerSeat = local_8;
                P_SEATSTATE(local_8) = 0;
                return 1;
            }
        }
    }
    return 0;
}
