Dispatcher_CreatePrivateStore::~Dispatcher_CreatePrivateStore() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0820271c orig/ours=287/296 ndiff-blocks=24
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: jne    0820274b <+0x2f> | je     0820277d <+0x61> | movl   $&_ZZN29Dispatcher_CreatePrivateStore12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION_
