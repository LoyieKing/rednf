// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::OnSetPVPSeatState（原 TU 第 1667 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-7 N95] ORIG 0x085dd070：局部槽 -0x10/-0x1c，4.4 -0xc/-0x24（槽基差 8）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮：P292/P248 档① check_authority int→bool（ORIG 调用点
// `xor $0x1; test %al,%al; je`）⇒ 254→253；残余 CheckSeat 需跨泳道 bool 化（已上报 Main）
// + N95 槽基差 8。
bool PvP_Room::OnSetPVPSeatState(CUser* user, int seat, ENUM_SEAT_STATE state,
                                ENUM_WALKOUT_TYPE walkoutType)
{
    CUser* local_24;
    bool local_25;
    bool local_26;
    bool local_27;
    int local_10;
    if (user == 0)
    {
        return 1;
    }
    PacketGuard local_20;
    int local_14 = 0;
    if (check_authority(user, seat) == 0)
    {
        local_14 = 8;
        goto LAB_085dd357;
    }
    if (((IMatch*)m_matching)->CheckSeat((int)state) == 0)
    {
        local_14 = 8;
        goto LAB_085dd357;
    }
    if (m_state == 2)
    {
        local_14 = 0x13;
        goto LAB_085dd357;
    }
    local_24 = 0;
    local_25 = false;
    local_26 = false;
    local_27 = false;
    local_10 = get_team(seat);
    if (state == ENUM_SEAT_STATE_0xfe)
    {
        if (walkoutType == (ENUM_WALKOUT_TYPE)3)
        {
            SendWalkOut(seat, walkoutType);
        }
        else if ((m_seats[7] == user) && (P_SEATS(seat) != 0) &&
                 (P_SEATS(seat) != user))
        {
            SendWalkOut(seat, walkoutType);
        }
    }
    local_14 = set_seat_state(user, seat, state, &local_24, local_25, local_26, local_27);
    if ((get_waiter_count() == 0) && (local_27 == false))
    {
        LogManager::logFormat(1, "pvp.cpp",
                              __PRETTY_FUNCTION__,
                              0x11be, "PvP Room(%d) is empty!!!", get_index());
    }
    if (local_14 == 0)
    {
        if (local_24 != 0)
        {
            ((IMatch*)m_matching)->OnLeaveRoom(local_24, this, local_10, seat);
            G_GameWorld()->out_from_pvp(local_24);
            local_24->insert_pvp_masterid_walkingout_me((unsigned)user->getCurCharacNo());
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
LAB_085dd357:
    if (local_14 > 0)
    {
        local_20.put_header(1, 0x37);
        local_20.put_byte(0);
        local_20.put_byte(local_14);
        local_20.finalize(true);
        user->Send(local_20);
    }
    return 1;
}
