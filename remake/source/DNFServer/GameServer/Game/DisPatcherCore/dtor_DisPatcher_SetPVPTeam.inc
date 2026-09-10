DisPatcher_SetPVPTeam::~DisPatcher_SetPVPTeam() {}
// ORIG 0x081f6020 T _ZN21DisPatcher_SetPVPTeam12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] P30/N4：ORIG 结构体基址 lea -0x2c + add $0xd 两步，4.4 折单 lea -0x21；槽号平移随 N82  (ORIG 0x081f6020 insn 137->139)
// 报告：docs/class_func_reports/DisPatcher_SetPVPTeam/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 75（ORIG 137 / ours 138），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
