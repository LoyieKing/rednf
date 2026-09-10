Dispatcher_BVHackInfo::~Dispatcher_BVHackInfo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08207568 orig/ours=196/190 ndiff-blocks=31
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN21Dispatcher_BVHackInfo12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $0x0,-0xc(%ebp) | lea    -0xc(%ebp),%ea
