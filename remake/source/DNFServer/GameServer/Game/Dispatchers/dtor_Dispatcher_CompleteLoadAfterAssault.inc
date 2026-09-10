Dispatcher_CompleteLoadAfterAssault::~Dispatcher_CompleteLoadAfterAssault() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08206b5c orig/ours=110/119 ndiff-blocks=19
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN35Dispatcher_CompleteLoadAfterAssault12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,%esi | call   080cc1
