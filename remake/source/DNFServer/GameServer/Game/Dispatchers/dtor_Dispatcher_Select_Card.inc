Dispatcher_Select_Card::~Dispatcher_Select_Card() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ff7de orig/ours=165/160 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movswl %ax,%ebx | sete   %al | test   %al,%al
