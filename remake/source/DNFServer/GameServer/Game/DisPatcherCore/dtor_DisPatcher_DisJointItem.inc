DisPatcher_DisJointItem::~DisPatcher_DisJointItem() {}
// ORIG 0x081f9fb8 T _ZN23DisPatcher_DisJointItem12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_DisJointItem/dispatch_sig.md 汇编+反编译 C 全文转写）：
// user 空或 CheckInTrade() 真 → return 0；依次 get_short(slot)/get_byte(type)/
// get_short(target,0xffff 初始)，失败分别 LineFunc 0x88d0/0x88d2/0x88d6；
// 全部成功 → _disjoint(user,(int)(short)slot,(int)(char)type,0x1c,0,(ushort)target)。
