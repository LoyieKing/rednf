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
