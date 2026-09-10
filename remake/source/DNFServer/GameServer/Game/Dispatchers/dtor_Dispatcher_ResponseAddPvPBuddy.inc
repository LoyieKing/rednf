Dispatcher_ResponseAddPvPBuddy::~Dispatcher_ResponseAddPvPBuddy() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08224ab4 orig/ours=117/92 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x36(%ebp),%eax | mov    $0x1e,%ebx | mov    $0x0,%edx
// [SweepDispatchers 2026-09-09] 同 RemovePvPBuddy：name 清零改 `char name[0x1e]={0}`（ORIG
//   0x08224ad2 内联慢路）并前置于 type/nameLen；92→117 = ORIG 117。
