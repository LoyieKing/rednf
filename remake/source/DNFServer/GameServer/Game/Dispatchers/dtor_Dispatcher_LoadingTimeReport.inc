Dispatcher_LoadingTimeReport::~Dispatcher_LoadingTimeReport() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0821efec orig/ours=361/351 ndiff-blocks=58
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x820(%ebp),%eax | movw   $0x0,-0x16(%ebp) | movl   $0x0,-0x14(%ebp)
