DisPatcher_DeleteCharac::~DisPatcher_DeleteCharac() {}
// ORIG 0x081c14f0 T _ZN23DisPatcher_DeleteCharac11check_errorEP5CUserR8MSG_BASE
// [b28d-29] N82/P0：msg 形参副本落 -0x18（ORIG -0x14 直读形），帧 0x44 vs 0x40，调用点全等  (ORIG 0x081c14f0 insn 199->198)
// 报告：docs/class_func_reports/DisPatcher_DeleteCharac/check_error.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 76（ORIG 199 / ours 197），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
