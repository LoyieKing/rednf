DisPatcher_Lottery::~DisPatcher_Lottery() {}
// [Lane5] ORIG 0x081f845b `lea -0x1f(%ebp),%eax; add $0xd,%eax`（get_short 目标）
// + 0x081f84a2 `movzwl -0x12(%ebp)` 读回 + PacketGuard@-0x2c ⇒ 原码有 0xf 字节
// pack(1) 请求体局部（ORIG 未出线，仅 m_0d 被读）。**实测**：加 0xf 视图结构体后
// 4.4 帧 0x50→0x60、PacketGuard -0x2c→-0x40、ndiff 133（劣化）⇒ 回退为裸 `short idx`；
// 残余为 N93/P221 互斥块对象槽向（ORIG guard1 浅/guard2 深，4.4 反向）。
// ORIG 0x081f8380 T _ZN18DisPatcher_Lottery12dispatch_sigEP5CUserR9PacketBuf
// [b28d-29] N82：帧内对象槽整体平移 8（ORIG -0x2c，4.4 -0x34），助记符序列与调用点全等  (ORIG 0x081f8380 insn 166->167)
// 报告：docs/class_func_reports/DisPatcher_Lottery/dispatch_sig.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 128（ORIG 166 / ours 166），残差=P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
