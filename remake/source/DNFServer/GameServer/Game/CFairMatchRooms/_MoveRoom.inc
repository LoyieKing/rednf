void CFairMatch::_MoveRoom(PvP_Room* to, PvP_Room* from, bool observerFlag)
{
    // ORIG 0x08563d32 +0x0e..+0x23 初始化序 = 声明序槽：
    // team→-0x18、mover→-0x14、newSeat→-0x10、seat(unsigned)→-0xc（setbe 实证）。
    int teamOf = 0;
    CUser* mover = 0;
    int newSeat = 0;
    for (unsigned seat = 0; seat < 8; ++seat) {
        mover = reinterpret_cast<CUser*>(from->get_user_seat(seat));
        if (mover == 0) continue;
        teamOf = from->get_team(seat);

        PacketGuard pkt;
        pkt.put_header(0, 0x29);
        pkt.put_short(1);
        to->make_room_info(reinterpret_cast<char*>(&pkt));  // ORIG +0xa3 0xc(%ebp)=to
        pkt.finalize(true);
        from->send_to_pvp(pkt);

        _PullOutRoom(mover, from, seat);
        _SetEnterCharacNo(mover->getCurCharacNo());
        newSeat = _PutInRoom(mover, to, observerFlag);
        if (teamOf == 3) {
            to->OnSetPVPSeatState(mover, newSeat,
                                  static_cast<ENUM_SEAT_STATE>(3),
                                  static_cast<ENUM_WALKOUT_TYPE>(2));
        }
    }
