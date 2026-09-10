Dispatcher_SecurityLog::~Dispatcher_SecurityLog() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825fb38 orig/ours=76/77 ndiff-blocks=12
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x24(%ebp) | mov    0x10(%ebp),%eax
