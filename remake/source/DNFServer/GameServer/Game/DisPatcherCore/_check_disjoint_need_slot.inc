void _check_disjoint_need_slot(CUser* user, CItem* item, Inven_Item& invenItem,
                               expert_job::CDisjointer* disjointer);

// §6.2 巨件核销：P167 98/98 [0缺0多]；gap 811→811/818；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] N82：帧 0x2bc vs 0x2dc + 槽平移（-0x26c vs -0x28c），调用点全等  (ORIG 0x081f92ca insn 811->818)
// 无真修条目。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 544（ORIG 807 / ours 815），残差=setcc-fold bool 物化岛/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
