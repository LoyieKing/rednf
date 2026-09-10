Dispatcher_Change_Charac_Slot::~Dispatcher_Change_Charac_Slot() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082251f4 orig/ours=79/92 ndiff-blocks=11
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: xor    $0x1,%eax | jne    0822524b <+0x57>
