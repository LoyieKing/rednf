Dispatcher_CompatibilityIndex::~Dispatcher_CompatibilityIndex() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e2664 orig/ours=78/110 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    %eax,-0xc(%ebp)
