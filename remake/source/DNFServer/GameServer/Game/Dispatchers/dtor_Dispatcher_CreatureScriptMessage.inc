Dispatcher_CreatureScriptMessage::~Dispatcher_CreatureScriptMessage() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08204d8e orig/ours=246/240 ndiff-blocks=22
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x157(%ebp),%eax | add    $0xd,%eax | movl   $&_ZZN32Dispatcher_CreatureScriptMessage12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0
