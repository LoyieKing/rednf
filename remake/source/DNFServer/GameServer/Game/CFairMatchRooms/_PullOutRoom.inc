void CFairMatch::_PullOutRoom(CUser* user, PvP_Room* room, int seat)
{
    // b28d-solo-CFairMatchRooms：P227 全表达式临时链 —— ORIG 在 ctor 前右起求
    // varargs 并跨 ctor 保活（edi/esi/ebx + -0x2c 槽）；具名 log 两语句形令 4.4
    // 先 ctor 后求值（帧 0x3c vs 0x4c）。
    CSwitchLog(
        "void fair_pvp::CFairMatch::_PullOutRoom(CUser*, PvP_Room*, int)",
        0xb5a, 0, 0)("pvp@log %s,%s,room(%d),%d,%d",
                     user->get_acc_name(), user->getCurCharacName(),
                     room->get_index(), room->get_pvp_battle_mode(), seat);
    // b28d-solo-CFairMatchRooms：P292 档①——ORIG 尾部 `xor $1,%eax; test %al,%al;
    // je`（bool callee 裸否定形，无 setne/槽物化）⇒ 源码直写 if(!call())，
    // 删具名 int ok 局部（该局部在 ORIG 无槽往返）。
    if (!room->OnSetPVPSeatState(
            user, seat,
            static_cast<ENUM_SEAT_STATE>(0xfe),
            static_cast<ENUM_WALKOUT_TYPE>(2))) {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
