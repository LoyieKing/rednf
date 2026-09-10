Dispatcher_UseCraneStart::~Dispatcher_UseCraneStart() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0821d2c4 orig/ours=152/150 ndiff-blocks=4
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN24Dispatcher_UseCraneStart12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZTI10ENUM_ERROR,0x4(%esp) | mov  
