// ORIG 0x081f5eba T _ZN21DisPatcher_SetPVPSeat12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetPVPSeat/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=5 → 0；get_byte(seat) 失败或 seat∉[0,7] → LineFunc 0x82a4；get_byte(state)
// 失败 → 0x82a6；room==0 → 0x82a9；OnSetPVPSeatState(user,(int)(char)seat,state,2)!=1
// [b28d-29] AE：91=91 全等（同上重定位归一）  (ORIG 0x081f5eba insn 91->91)
// → room->reset() + G_CGameManager()->PutPvp(room)；返回 0。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 91 / ours 91 insn 全等（AE 口径，重定位归一）。
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
#pragma pack(push, 1)
    struct SetPVPSeatMsg {
        char m_hdr[0xd];
        char m_seat;
        unsigned char m_state;
    };
#pragma pack(pop)
    SetPVPSeatMsg msg = {};
    {
        char seat = 0;
        if (!packet.get_byte(seat) || seat < 0 || seat > 7)
        {
            return LineFunc(0x82a4, __PRETTY_FUNCTION__, 0, 0);
        }
        msg.m_seat = seat;
    }
    {
        unsigned char state = 0;
        if (!packet.get_byte(state))
        {
            return LineFunc(0x82a6, __PRETTY_FUNCTION__, 0, 0);
        }
        msg.m_state = state;
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x82a9, __PRETTY_FUNCTION__, 0, 0);
    }
    // seat=msg[13]@-0xe `(int)(char)` sext（`movsbl %al,%eax`→0x8(%esp)）；
    // state=msg[14]@-0xd `(int)(unsigned char)` 双零扩展（`movzbl %al,%edx`→0xc(%esp)）。
    // ORIG 0x081f5eba：call 后 `xor $0x1,%eax; test %al,%al; je`（技1 bool 取反）。
    if (!room->OnSetPVPSeatState(user, (int)msg.m_seat,
                                 (ENUM_SEAT_STATE)(int)msg.m_state,
                                 (ENUM_WALKOUT_TYPE)2))
    {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
    return 0;
}
