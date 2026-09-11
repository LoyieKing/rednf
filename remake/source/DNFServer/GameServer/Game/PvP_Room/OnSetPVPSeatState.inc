// [b28d-7 N95] ORIG 0x085dd070：局部槽 -0x10/-0x1c，4.4 -0xc/-0x24（槽基差 8）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] 本轮：P292/P248 档① check_authority int→bool（ORIG 调用点
// `xor $0x1; test %al,%al; je`）⇒ 254→253；残余 CheckSeat 需跨泳道 bool 化（已上报 Main）
// + N95 槽基差 8。
// [L11 已 identical 2026-09-11] 旧判 N95「槽基差 8 封顶」**推翻**：真因是**声明位置**——
//   `PacketGuard local_20` / `CUser* local_24` / `bool local_25..27` 由函数头移入
//   `if (user == 0)` 之后、`int local_10` 移到 `int local_14` 之后 ⇒ 五局部槽全部归位
//   （ORIG -0x1c/-0x20/-0x21..-0x23/-0x10/-0xc，帧 $0x40 不变）。252/252 gap=0 ndiff=0。
//   范式 **P-L11c**：4.4 槽分配随**声明位置**整体重排，跨版本槽基差可翻（勿再判 N95 封顶）。
bool PvP_Room::OnSetPVPSeatState(CUser* user, int seat, ENUM_SEAT_STATE state,
                                ENUM_WALKOUT_TYPE walkoutType)
{
    if (user == 0)
    {
        return 1;
    }
    PacketGuard local_20;
    CUser* local_24;
    bool local_25;
    bool local_26;
    bool local_27;
    int local_14 = 0;
    int local_10;
    // [H8-PvP 2026-09-11 · P292/P248 档① 实测中性，已回滚] ORIG 0x085dd0ae
    // `call check_authority; xor $0x1,%eax; test %al,%al; je <fail>` 看似 `!= 1`
    // 形，但 4.4 对 `== 0` 与 `!= 1`（callee 声明已是 bool）产**逐条相同**代码：
    // 本件 252=252、ndiff 106/106 逐值不变 ⇒ 非源码可控（N95 槽基差族）。
    if (check_authority(user, seat) == 0)
    {
        local_14 = 8;
        goto LAB_085dd357;
    }
    // 同上（第二处同测中性，已回滚）；CheckSeat vptr 偏移 +0x74 已核
    // MatchRoomList.h:86 `virtual bool CheckSeat(int) ... // +0x74` ⇒ 无偏移 bug。
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
                              // [H8-PvP 2026-09-11 · S2 语义轴] 同 S3：ORIG 0x085dd227
                              // `movl $0x8cc12fc,0x10(%esp)`（与 WalkOut 共用同一
                              // .rodata 串）⇒ 换成韩文 + " ID[%d]"。
                              0x11be, "\xb9\xe6\xbe\xf8\xbe\xee\xc1\xae\xbe\xdf\xc7\xcf\xb4\xc2\xb5\xa5 \xbe\xc8\xbe\xf8\xbe\xee\xc1\xfc ID[%d]", get_index());
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