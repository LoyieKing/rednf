Dispatcher_QueryCharacInfo::~Dispatcher_QueryCharacInfo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08205f68 orig/ours=120/90 ndiff-blocks=10
//   帧差 sub $40 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x40,%esp
// [SweepDispatchers 2026-09-09] 形态收敛（90→120 = ORIG 120；p167 12/12 CLEAR）：
//   ORIG 0x08205fc8 先 `len=0` 再 0x08205fcf name 清零（4.1 内联对齐剥离慢路）；
//   旧码 memset call + 顺序颠倒。改 `int len = 0; char name[0x1e] = {0};` 后指令数全等。
