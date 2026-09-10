Dispatcher_TitleBookGet::~Dispatcher_TitleBookGet() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d7ada orig/ours=154/164 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0xd(%eax),%eax | mov    0x15(%eax),%ecx | mov    0x1d(%eax),%edx
