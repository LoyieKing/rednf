Dispatcher_RemovePvPBuddy::~Dispatcher_RemovePvPBuddy() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08224c5e orig/ours=117/92 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x36(%ebp),%eax | mov    $0x1e,%ebx | mov    $0x0,%edx
// [SweepDispatchers 2026-09-09] 形态收敛（92→117 = ORIG 117）：ORIG 0x08224c85 name 清零为
//   4.1 内联对齐剥离慢路（旧码 memset call，p167 多重集缺 25 条）⇒ 改 `char name[0x1e]={0}`
//   并把 name 声明提到 type/nameLen 之前（ORIG 清零点 0x027 先于 0x079/0x07d）。
