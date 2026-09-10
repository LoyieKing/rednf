PvP_Room* CFairMatch::_ReturnTeam(PvP_Room* room, int teamNo, CUser* excluded)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链（_PullOutRoom 实证同族）。
    CSwitchLog(
        "PvP_Room* fair_pvp::CFairMatch::_ReturnTeam(PvP_Room*, int, CUser*)",
        0xd95, 0, 0)("pvp@log room(%d), %d, %d",
                     room->get_index(), room->get_pvp_battle_mode(),
                     room->get_waiter_count());
    CUser* member = 0;
    PvP_Room* newRoom = 0;
    // b28d-solo-CFairMatchRooms：P196 —— ORIG 循环尾测为 setbe（无符号），
    // 源码 seat 须为 unsigned（int 形 4.4 恒 setle）。
    for (unsigned int seat = 0; seat <= 7; ++seat) {
        member = reinterpret_cast<CUser*>(room->get_user_seat(seat));
        if (member == 0) continue;
        const int teamOf = room->get_team(seat);
        if (teamNo == teamOf) {
            if (teamNo == 2) {   // 观战队：直接按 walkoutType=2 清座
                room->OnSetPVPSeatState(
                    member, seat,
                    static_cast<ENUM_SEAT_STATE>(1),
                    static_cast<ENUM_WALKOUT_TYPE>(2));
            }
            continue;   // asm: 命中本队后不搬
        }
        if (excluded != 0 && member == excluded)
            continue;

        _PullOutRoom(member, room, seat);
        if (newRoom == 0) {
            newRoom = _MakeRoom(member);
        } else {
            _PutInRoom(member, newRoom, true);   // asm 第3参恒 1
        }
    }
    m_field04 = 0;
    return newRoom;
