Dispatcher_SecurityCardCertKey::~Dispatcher_SecurityCardCertKey() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0820a6bc orig/ours=173/182 ndiff-blocks=21
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN30Dispatcher_SecurityCardCertKey12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $0x0,-0x10(%ebp) | movl   $0x0
