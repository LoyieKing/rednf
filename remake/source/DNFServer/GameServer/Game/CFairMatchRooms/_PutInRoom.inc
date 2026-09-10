int CFairMatch::_PutInRoom(CUser* user, PvP_Room* room, bool observer)
{
    PacketGuard pkt;
    int seat = 0;
    const int result = room->join_room(user, seat, observer);
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链（_PullOutRoom 实证同族）。
    CSwitchLog(
        "int fair_pvp::CFairMatch::_PutInRoom(CUser*, PvP_Room*, bool)",
        0xb6a, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                     user->get_acc_name(), user->getCurCharacName(),
                     room->get_index(), room->get_pvp_battle_mode(), result);
    if (result >= 1) {
        pkt.put_header(1, 0x36);
        pkt.put_byte(0);
        // b28d-solo-CFairMatchRooms：P162 —— ORIG 直接传 int（无 movzbl），
        // 源码不做 (unsigned char) 收窄。
        pkt.put_byte(result);
        pkt.finalize(true);
        user->Send(pkt);
        // ORIG @+0x81 `mov -0x3c(%ebp),%ebx; jmp` ⇒ 成功分支内独立 return
        // （两处 return seat 各自物化 ebx）。
        return seat;
    } else {
        // ORIG: make_seat_info(room,(char*)&pkt,seat) —— PacketGuard 对象
        // 地址被传入（无具名指针局部，P0/P89），函数内部经其
        // InterfacePacketBuf 基类写包。
        room->make_seat_info(reinterpret_cast<char*>(&pkt), seat);
        G_GameWorld()->send_all(pkt);
        G_GameWorld()->goto_pvp(user);
    }
    // b28d-167：p167 差集 EXTRA InterfacePacketBuf::finalize×1 ⇒ ORIG 无尾部
    // finalize（两条路径各自 finalize 后返回），删除。
    return seat;
