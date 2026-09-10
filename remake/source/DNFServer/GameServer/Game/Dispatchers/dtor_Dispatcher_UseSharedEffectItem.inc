Dispatcher_UseSharedEffectItem::~Dispatcher_UseSharedEffectItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08220484 orig/ours=238/238 ndiff-blocks=40
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $0x0,-0x18(%ebp) | movw   $0x0,-0x1a(%ebp) | lea    -0x18(%ebp),%eax
