Dispatcher_Lag_Statistics::~Dispatcher_Lag_Statistics() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08223de0 orig/ours=416/398 ndiff-blocks=66
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x15d(%ebp),%eax | movb   $0x0,-0x11(%ebp) | lea    -0x15d(%ebp),%eax
