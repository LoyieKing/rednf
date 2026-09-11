// [S1Charac 2026-09-11 复核] 652=652、无指令增删：diff 257+/257- 全为 call/跳转落点
// （begin/de/ppEi/ctor 调用块的**位置重排**，如 ORIG +0x57e `jmp` vs 我方 +0x5a 段）
// ⇒ 4.4 循环块调度差 + N95 槽号整体平移；§6.5：本函数无 rodata 串引用 ⇒ CLEAR。
void createStackableMultiBoxLotteryInfo(
