Dispatcher_CheckGuildAddressDouble::~Dispatcher_CheckGuildAddressDouble() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082087a4 orig/ours=159/159 ndiff-blocks=23
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN34Dispatcher_CheckGuildAddressDouble12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $0x0,-0x14(%ebp) | lea    
