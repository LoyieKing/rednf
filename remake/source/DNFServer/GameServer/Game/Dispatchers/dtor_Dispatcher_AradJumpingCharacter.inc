Dispatcher_AradJumpingCharacter::~Dispatcher_AradJumpingCharacter() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0819f1ec orig/ours=172/177 ndiff-blocks=24
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x19(%ebp),%eax | lea    -0x19(%ebp),%eax | movzbl -0x10(%ebp),%eax
