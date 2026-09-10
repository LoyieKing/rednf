Dispatcher_CheckGuildNameDouble::~Dispatcher_CheckGuildNameDouble() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082084e8 orig/ours=172/170 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN31Dispatcher_CheckGuildNameDouble12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN31Dispatcher_CheckGuild
