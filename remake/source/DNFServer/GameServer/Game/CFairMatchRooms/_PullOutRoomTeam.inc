int CFairMatch::_PullOutRoomTeam(PvP_Room* room, ENUM_SCHOOL_MATCH_TEAM teamIdx)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链（_PullOutRoom 实证同族）。
    CSwitchLog(
        "bool fair_pvp::CFairMatch::_PullOutRoomTeam(PvP_Room*, ENUM_SCHOOL_MATCH_TEAM)",
        0xfa7, 0, 0)("pvp@log room(%d),%d,%d",
                     room->get_index(), room->get_pvp_battle_mode(),
                     static_cast<int>(teamIdx));
    int lastTeam = 0;
    for (int i = 0; i < 4; ++i) {
        // b28d-solo-CFairMatchRooms：P157/P183 去引用缓存（ORIG 每处字段访问
        // 重算完整地址，无引用槽）。
        if (m_teams[teamIdx].m_players[i].m_flag != 0 &&
            m_teams[teamIdx].m_players[i].m_user != 0) {
            const int seat =
                room->get_user_seat(m_teams[teamIdx].m_players[i].m_user);
            if (seat != -1) {
                lastTeam = room->get_team(seat);
                _PullOutRoom(m_teams[teamIdx].m_players[i].m_user, room, seat);
            }
        }
    }
    if (lastTeam == 1) {
        for (int seat = 0; seat <= 7; ++seat) {
            CUser* u = reinterpret_cast<CUser*>(room->get_user_seat(seat));
            // b28d-solo-CFairMatchRooms：P225/P295 —— ORIG `cmp $2; sete %al;
            // test %al,%al; je`（bool 物化）；`&&` 短路形被 4.4 折成 jne 直跳，
            // 改 while+break 复现物化。
            if (u != 0) {
                while (room->get_team(seat) == 2) {
                    room->OnSetPVPSeatState(u, seat,
                                            static_cast<ENUM_SEAT_STATE>(1),
                                            static_cast<ENUM_WALKOUT_TYPE>(2));
                    break;
                }
            }
        }
    }
    return 1;
