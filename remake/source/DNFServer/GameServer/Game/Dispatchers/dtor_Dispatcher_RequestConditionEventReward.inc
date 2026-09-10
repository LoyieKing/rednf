Dispatcher_RequestConditionEventReward::~Dispatcher_RequestConditionEventReward() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082223a6 orig/ours=211/221 ndiff-blocks=32
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN38Dispatcher_RequestConditionEventReward12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movw   $0x0,-0x16(%ebp) | mov
