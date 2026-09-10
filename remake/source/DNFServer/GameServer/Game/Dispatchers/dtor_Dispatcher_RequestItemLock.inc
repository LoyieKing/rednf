Dispatcher_RequestItemLock::~Dispatcher_RequestItemLock() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08222e00 orig/ours=97/93 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movb   $0x0,-0x11(%ebp) | movw   $0x0,-0x14(%ebp) | lea    -0x11(%ebp),%eax
