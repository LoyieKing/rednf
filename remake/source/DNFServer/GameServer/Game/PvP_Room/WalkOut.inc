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
                              // [H8-PvP 2026-09-11 · S3 语义轴] ORIG 0x085dd570
                              // `movl $0x8cc12fc,0x10(%esp)`（.rodata 逐字节：
                              // b9 e6 be f8 be ee c1 ae be df c7 cf b4 c2 b5 a5 20
                              // be c8 be f8 be ee c1 fc 20 49 44 5b 25 64 5d 00）
                              // ⇒ 原串为韩文 + " ID[%d]"；旧 ASCII 串是发明文本。
                              0x1210, "\xb9\xe6\xbe\xf8\xbe\xee\xc1\xae\xbe\xdf\xc7\xcf\xb4\xc2\xb5\xa5 \xbe\xc8\xbe\xf8\xbe\xee\xc1\xfc ID[%d]", get_index());
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