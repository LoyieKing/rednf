// ---------------------------------------------------------------------------
// 4. _ReturnTeam —— ORIG 0x08561e40 逐指令转写。
//    ORIG 帧：-0x18 = seat；-0x14 = newRoom；-0x10 = seatIdx；-0xc = team。
// ---------------------------------------------------------------------------
// [b28d-solo-MatchRoomLCB] AE（check_tu_routed: ae=1；.o size 0x125 = ORIG 0x125）。
//   ORIG 0x08561e40 逐指令转写（79 insn；残差仅 .o 重定位标签伪差，指令偏移
//   +0x10a/+0xff/+0x102/+0x91/... 逐一对应）。p167 CLEAR（6/6）。
//   槽位：-0x18 seat / -0x14 newRoom / -0x10 seatIdx / -0xc curTeam。
//   真语义修正（ORIG 实证）：get_user_seat 单次调用并缓存 seat，四处下游调用
//   （OnSetPVPSeatState/_PullOutRoom/_MakeRoom/_PutInRoom）第 1/2 实参均为
//   **seat**（ORIG 压 -0x18）而非形参 user；旧稿误传 user 且把
//   `team==curTeam && team!=2` 并入 else 分支（ORIG +0x60 jne 岛2 = continue）。
//   seatIdx 为 unsigned（ORIG 尾测 setbe），仅在 for 内单次初始化（ORIG 头
//   只有一条 movl $0x0,-0x10 —— 头部再声明一条会多 1 条 DIFF）。
void COnlinePreliminary::_ReturnTeam(PvP_Room* room, int team, CUser* user)
{
    CUser* seat = 0;                 // -0x18（get_user_seat 结果，ABI 按 int 承载指针）
    PvP_Room* newRoom = 0;           // -0x14
    for (unsigned int seatIdx = 0; seatIdx <= 7; ++seatIdx) {  // 尾测 cmpl $7; setbe
        seat = (CUser*)room->get_user_seat(seatIdx);
        if (seat == 0)
            continue;                                          // 岛1（+0xff nop;jmp）

        const int curTeam = room->get_team(seatIdx);           // -0xc
        if (team == curTeam) {
            if (team != 2)
                continue;                                      // 岛2（+0x102 nop;jmp）
            room->OnSetPVPSeatState(seat, seatIdx,
                                    (ENUM_SEAT_STATE)1, (ENUM_WALKOUT_TYPE)2);
        } else {
            // 排除席（ORIG 0x8561ed1..：cmpl $0,user; je 续; cmp -0x18,user;
            // je skip）。get_user_seat(int) 槽位实际存放 CUser*，直接指针比较。
            if (user != 0 && seat == user)
                continue;                                      // 岛3（+0x105 nop）

            _PullOutRoom(seat, room, seatIdx);
            if (newRoom == 0)
                newRoom = _MakeRoom(seat);                     // ORIG 返回 PvP_Room*
            else
                _PutInRoom(seat, newRoom, true);               // 观战者入新房间
        }
    }
    m_field04 = 0;
}