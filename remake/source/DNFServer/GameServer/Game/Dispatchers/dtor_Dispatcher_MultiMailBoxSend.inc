Dispatcher_MultiMailBoxSend::~Dispatcher_MultiMailBoxSend() {}
// ORIG T 0x081202a9：交易限次物品移动资格。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081202a9 orig/ours=50/52 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | cmpl   $0x0,-0x10(%ebp) | je     08120321 <+0x78>
