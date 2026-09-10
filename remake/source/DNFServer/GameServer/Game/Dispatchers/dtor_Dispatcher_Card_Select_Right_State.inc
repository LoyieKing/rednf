Dispatcher_Card_Select_Right_State::~Dispatcher_Card_Select_Right_State() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ff504 orig/ours=193/190 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: sete   %al | test   %al,%al | je     081ff5a2 <+0x9e>
