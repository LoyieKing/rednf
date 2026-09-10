Dispatcher_RequestDisjointItem::~Dispatcher_RequestDisjointItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cfcac orig/ours=70/68 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp) | mov    %eax,%edx
