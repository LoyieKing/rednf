DisPatcher_RepairEquip::~DisPatcher_RepairEquip() {}
// [b28d-solo-DisPatcherCore] 复核 DIFF：正极性形实测 32→31 insn（ORIG 28）；
// 尾段已与 ORIG 同形（`test; jne <mov $0>` + fall `mov $0x13`），残余 3 条：
// ① 4.4 把 register state 落 callee-saved %esi（ORIG 留 %eax：call 返回值直判+
// `mov %eax,%ecx` 复用）⇒ push/pop esi + mov/cmp/mov 共 4 条；② ORIG `mov $1,%edx;
// mov %edx,%ebx` 立即数物化 2 条 vs 4.4 `mov $1,%ebx` 1 条；③ ja 落点块布局。
// 两句式/goto 形均劣 ⇒ N7+N18a 封顶。
