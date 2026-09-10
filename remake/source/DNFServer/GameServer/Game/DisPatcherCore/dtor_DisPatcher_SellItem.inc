DisPatcher_SellItem::~DisPatcher_SellItem() {}
// §6.2 巨件核销：P167 77/77 [0缺0多]；gap 515→515/522；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] P30/N4：ORIG 结构体基址 lea -0x52 + add $0xd/0xe/0x10 两步，4.4 折单 lea；语义集合全等  (ORIG 0x081f2d0c insn 515->522)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 287（ORIG 515 / ours 522），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
