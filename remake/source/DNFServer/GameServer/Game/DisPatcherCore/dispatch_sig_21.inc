int DisPatcher_SetPVPSeat::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 5)
    {
        return 0;
    }
    // ORIG 0x081f5eba：blob `char msg[15]`@-0x1b（帧 0x48），实参两段
    // `lea -0x1b(%ebp); add $0xd/0xe`（技26）；seat=msg[13]@-0xe、state=msg[14]@-0xd。
    // 尾读 seat `movzbl -0xe,%eax; movzbl %al,%edx`（技16 uchar→int 双零扩展）、
    // state `movzbl -0xd,%eax; movsbl %al,%eax`（(char) sext）。
    char msg[15];
    if (!packet.get_byte(*reinterpret_cast<char*>(msg + 0xd)) ||
        (char)msg[13] < 0 || (char)msg[13] > 7)
    {
        return LineFunc(0x82a4, __PRETTY_FUNCTION__, 0, 0);
    }
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 第二处调 `get_byte(unsigned char&)`
    // （_ZN9PacketBuf8get_byteERh），原写法为 char&（ERc）。
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(msg + 0xe)))
    {
        return LineFunc(0x82a6, __PRETTY_FUNCTION__, 0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x82a9, __PRETTY_FUNCTION__, 0, 0);
    }
    // seat=msg[13]@-0xe `(int)(char)` sext（`movsbl %al,%eax`→0x8(%esp)）；
    // state=msg[14]@-0xd `(int)(unsigned char)` 双零扩展（`movzbl %al,%edx`→0xc(%esp)）。
    // ORIG 0x081f5eba：call 后 `xor $0x1,%eax; test %al,%al; je`（技1 bool 取反）。
    if (!room->OnSetPVPSeatState(user, (int)(char)msg[13],
                                 (ENUM_SEAT_STATE)(int)(unsigned char)msg[14],
                                 (ENUM_WALKOUT_TYPE)2))
    {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
    return 0;
