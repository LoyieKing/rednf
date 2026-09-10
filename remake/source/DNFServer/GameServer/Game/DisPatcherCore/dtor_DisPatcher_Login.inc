DisPatcher_Login::~DisPatcher_Login() {}
// §6.2 巨件核销：P167 168/168 [0缺0多]；gap 986→986/990；残差归因 N82/P112
// （槽复用策略 + 块排布，diff 形态计数对称无缺类，详见
// [b28d-29] N82：帧 0x1c0 vs 0x1d0 + 局部对象槽平移（-0xc4 vs -0x90），调用点全等  (ORIG 0x081e8c78 insn 986->990)
// docs/asm_translation_patterns.md 负例表）。无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 704（ORIG 986 / ours 990），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
