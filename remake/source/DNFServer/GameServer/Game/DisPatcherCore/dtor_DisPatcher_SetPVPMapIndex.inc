DisPatcher_SetPVPMapIndex::~DisPatcher_SetPVPMapIndex() {}
// ORIG 0x081f8602 T _ZN25DisPatcher_SetPVPMapIndex12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82 + setcc-fold：帧 0x40 vs 0x30；`!flag` 已按 P168a 修正（xor 形），残余为槽平移与 4.4 sete 折痕  (ORIG 0x081f8602 insn 175->179)
// 报告：docs/class_func_reports/DisPatcher_SetPVPMapIndex/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 59（ORIG 175 / ours 178），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
