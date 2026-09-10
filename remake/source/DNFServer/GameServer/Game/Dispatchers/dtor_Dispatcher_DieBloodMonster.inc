Dispatcher_DieBloodMonster::~Dispatcher_DieBloodMonster() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08220856 orig/ours=134/110 ndiff-blocks=16
//   帧差 sub $220 vs $238 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %ebx | sub    $0x220,%esp
// [SweepDispatchers 2026-09-09] 形态收敛（110→137，ORIG 134；多重集差仅 1 movswl）：
//   ① monsterIdx[256] 改 `= {0}` 复现 ORIG 0x082208d1 内联慢路（旧码 memset call，缺 24 条）；
//   ② 循环条件改 `(int)monsterCount > i`：ORIG 0x082209b6 `movzbl; movzbl %al; cmp i; setg`。
