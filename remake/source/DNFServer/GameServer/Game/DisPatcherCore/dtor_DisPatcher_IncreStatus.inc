DisPatcher_IncreStatus::~DisPatcher_IncreStatus() {}
// ORIG 0x081ede34 T _ZN22DisPatcher_IncreStatus12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_IncreStatus/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state<=2 → 0；get_short 失败 → LineFunc 0x70fc；GetItemPtr(user,0,slot)==0 →
// LineFunc 0x7101；CheckItem 0x2a 非0 → SendCmdErrorPacket(0x20,err)+0；
// [b28d-29] N82：帧 0x50 vs 0x40 + 槽平移（ORIG 结构体基址 -0x2f+add 两步，4.4 折 lea），语义集合全等  (ORIG 0x081ede34 insn 123->121)
// slot<0x99 → increase_status(slot)，否则 UseQuestItem(slot)+cMyTrace(0x710f,5)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 52（ORIG 123 / ours 121），残差=P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
