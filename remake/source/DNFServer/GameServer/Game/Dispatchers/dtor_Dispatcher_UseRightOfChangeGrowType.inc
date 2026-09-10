Dispatcher_UseRightOfChangeGrowType::~Dispatcher_UseRightOfChangeGrowType() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825dd84 orig/ours=124/130 ndiff-blocks=11
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: jne    0825dda7 <+0x23> | setne  %al | mov    0x10(%ebp),%eax
