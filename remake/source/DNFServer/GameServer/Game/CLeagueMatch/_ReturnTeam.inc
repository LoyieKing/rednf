// [b28d-solo-CLeagueMatch] AE（TU 口径）：79=79 全等。修法=P223② 显式提前
// `continue`（同层多 continue 复现 ORIG 0x0855e477/e47a/e47d 三个独立
// nop+jmp 中继岛；else-if 合并形被 4.4 jump-thread 少 2 岛，gap 2→0）。
void CLeagueMatch::_ReturnTeam(PvP_Room* room, int team, CUser* except)  // ORIG T 0x0855e378
{
    // [b28d-solo-CLeagueMatch] P0：ORIG 槽序 user(-0x18)/madeRoom(-0x14)/seat(-0x10)
    // ⇒ user 声明于函数头并置 0（有槽往返=具名局部），循环内赋值。
    CUser* user = 0;
    PvP_Room* madeRoom = 0;
    for (unsigned int seat = 0; seat <= 7; ++seat) {
        user = reinterpret_cast<CUser*>(room->get_user_seat(static_cast<int>(seat)));
        if (user == 0)
            continue;
        int seatTeam = room->get_team(static_cast<int>(seat));
        if (team == seatTeam) {
            // [b28d-solo-CLeagueMatch] P223②：ORIG 0x0855e47a/e47d 两个独立
            // nop+jmp 中继岛 = 同层多 `continue`（提前 continue 形），
            // else-if 合并形被 4.4 jump-thread 少 2 岛。
            if (team != 2)
                continue;
            room->OnSetPVPSeatState(user, static_cast<int>(seat),
                                    static_cast<ENUM_SEAT_STATE>(1),
                                    static_cast<ENUM_WALKOUT_TYPE>(2));
            continue;
        }
        if (except != 0 && user == except)
            continue;
        _PullOutRoom(user, room, static_cast<int>(seat));
        if (madeRoom == 0)
            madeRoom = _MakeRoom(user);
        else
            _PutInRoom(user, madeRoom, true);
    }
    m_state = 0;
}