// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::WalkOut（原 TU 第 1618 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

bool PvP_Room::WalkOut(CUser* user, int seat, ENUM_SEAT_STATE state,
                      ENUM_WALKOUT_TYPE walkoutType)
{
    // 残余为 N7 互换（虚调用 callee 寄存器跨版本分配，见 get_user_list 注释）。
    PacketGuard local_20;
    CUser* local_24 = 0;
    bool local_25 = false;
    bool local_26 = false;
    bool local_27 = false;
    int local_14 = get_team(seat);
    SendWalkOut(seat, walkoutType);
    int local_10 = set_seat_state(user, seat, state, &local_24, local_25, local_26, local_27);
    if ((get_waiter_count() == 0) && (local_27 == false))
    {
        LogManager::logFormat(1, "pvp.cpp",
                              __PRETTY_FUNCTION__,
                              0x1210, "PvP Room(%d) is empty!!!", get_index());
    }
    if (local_10 == 0)
    {
        if (local_24 != 0)
        {
            ((IMatch*)m_matching)->OnLeaveRoom(local_24, this, local_14, seat);
            G_GameWorld()->out_from_pvp(local_24);
        }
        if (local_25 != 0)
        {
            local_20.clear();
            make_seat_info((char*)&local_20, seat);
            G_GameWorld()->send_all(local_20);
        }
        if (local_26 != 0)
        {
            local_20.clear();
            make_state_info((char*)&local_20);
            G_GameWorld()->send_all(local_20);
        }
        if (local_27 != 0)
        {
            return 0;
        }
    }
    return 1;
}
