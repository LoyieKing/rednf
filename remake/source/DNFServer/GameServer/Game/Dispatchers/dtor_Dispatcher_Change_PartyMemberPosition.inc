Dispatcher_Change_PartyMemberPosition::~Dispatcher_Change_PartyMemberPosition() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0822759c orig/ours=189/189 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN37Dispatcher_Change_PartyMemberPosition12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN37Dispatcher_Chan
