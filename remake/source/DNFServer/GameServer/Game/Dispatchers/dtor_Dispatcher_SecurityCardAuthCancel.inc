Dispatcher_SecurityCardAuthCancel::~Dispatcher_SecurityCardAuthCancel() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0820a42c orig/ours=88/88 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN33Dispatcher_SecurityCardAuthCancel12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
