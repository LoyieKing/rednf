DisPatcher_UseStack::~DisPatcher_UseStack() {}
// ============================================================================
// ORIG T 0x081f63c0 _ZN19DisPatcher_UseStack12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_UseStack/dispatch_sig.md，1320 insn）
// 读 slot(+0)/itemSpace?/itemId/slotIdx/checksum → state 白名单（5,7,8,3,6,
// 10,0xc）→ checksum 反作弊 0x25b → 封印限制 0xd1 → GetItemPtr/CheckItem(0x2a)
// → 可堆叠使用地点 → 组队/TOD/锦标赛限定（错误包 header(1,0x2f) 系列尾部）
// → use_stackable 落地。
// ============================================================================
// §6.2 巨件核销：P167 208/208 [0缺0多]；gap 1320→1320/1321；残差归因 N82/P112
// （diff 形态计数对称无缺类，详见 docs/asm_translation_patterns.md 负例表）。
// [b28d-29] N82/P183：帧 0x120 vs 0x140 + ORIG 两步 add 寻址 vs 4.4 独立局部槽；语义集合全等  (ORIG 0x081f63c0 insn 1320->1321)
// 无真修条目（batch-20 半程核销语义未回退）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 1374（ORIG 1307 / ours 1321），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [IG3-semantic] 差一候选our `cmpl $0x1e6c` ↔ ORIG `cmp $0x1e6d`（ORIG 0x081f6c30）逐点核实为**等价，记 N**：
//   本文件 9654 行源码写 `itemId >= 0x1e6d`；ORIG 决策树 `cmp $0x1e6d,%eax; jb 0x81f6c4d(default)`，
//   4.4 对无符号 `>=` 发对偶 `cmpl $0x1e6c; jbe 下一比较`（Ghidra C `uVar1 < 0x1e6d ⇒ goto LAB_081f6c4d` 互证）；
//   v<=0x1e6c 路径经 `cmp $0x28978a`（恒不等）后仍落同一错误包块（0x81f6c4d ↔ 我方 183e5 段），无可见行为差。
//   同函数扫描：我方多出的比较常量仅此一处（非发明防御）；ORIG 多的 0x16/0x18/0x33 各 1 次
//   = 4.1 switch 树「je + 范围判定」重复比较，检查常量集合两侧一致（非缺失校验），故不改源码。
