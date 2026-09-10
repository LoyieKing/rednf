DisPatcher_EndPvPResult::~DisPatcher_EndPvPResult() {}
// ORIG 0x081f88ae T _ZN23DisPatcher_EndPvPResult12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_EndPvPResult/dispatch_sig.md 汇编+反编译 C 全文转写）：
// state!=7 → 0；room==0 → LineFunc 0x8681；set_recv_pvp_end_result_flag 后
// recv_all 为真 → end_pvp_result + make_state_info/send_all + end_pvp + battle_reset；
// 否则 isRematchable&&recv_team 命中时对 m_matching(+0x6e4) vtable+0x80 槽直调
// [b28d-29] setcc-fold + N82：bool 物化方向（ORIG mov $0,%eax 岛，4.4 %ebx）+ 块位置，语义集合全等  (ORIG 0x081f88ae insn 130->136)
// （room==0 → logFormat 0x86b6），未命中 count==1 时插 end_result 定时器。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 35（ORIG 130 / ours 135），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
