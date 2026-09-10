int CLeagueMatch::_PutInRoom(CUser* user, PvP_Room* room, bool observer)  // ORIG T 0x0855d42a
{
    // [G9WorldTimerLeague·CLMSub 2026-09-10] N104 现场复现（ndiff 19→**0**，
    // 85/85 全等；TU ae 62→63 / diff 17→16）：ORIG 0x0855d42a 段**两个出口分支
    // 各自**带 `mov -0x1c(%ebp),%ebx; jmp <尾块>`（4.1 不做分支尾合并），原
    // `if/else + 落尾 return` 形使 4.4 只在尾块头发一条 mov 且 else 侧直落 ⇒
    // gap 2。改「删 else、两分支各自 `return seatNo;`」后 4.4 产同形双出口。
    PacketGuard pkt;
    int seatNo = 0;
    int err = room->join_room(user, seatNo, observer);
    if (err > 0) {
        pkt.put_header(1, 0x36);
        pkt.put_byte(0);
        pkt.put_byte(err);
        pkt.finalize(true);
        user->Send(pkt);
        return seatNo;
    }
    room->make_seat_info(reinterpret_cast<char*>(&pkt), seatNo);
    G_GameWorld()->send_all(pkt);
    G_GameWorld()->goto_pvp(user);
    return seatNo;
