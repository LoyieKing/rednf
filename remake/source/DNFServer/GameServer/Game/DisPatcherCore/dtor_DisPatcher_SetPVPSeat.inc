DisPatcher_SetPVPSeat::~DisPatcher_SetPVPSeat() {}
// ORIG 0x081f5eba T _ZN21DisPatcher_SetPVPSeat12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_SetPVPSeat/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=5 → 0；get_byte(seat) 失败或 seat∉[0,7] → LineFunc 0x82a4；get_byte(state)
// 失败 → 0x82a6；room==0 → 0x82a9；OnSetPVPSeatState(user,(int)(char)seat,state,2)!=1
// [b28d-29] AE：91=91 全等（同上重定位归一）  (ORIG 0x081f5eba insn 91->91)
// → room->reset() + G_CGameManager()->PutPvp(room)；返回 0。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 91 / ours 91 insn 全等（AE 口径，重定位归一）。
