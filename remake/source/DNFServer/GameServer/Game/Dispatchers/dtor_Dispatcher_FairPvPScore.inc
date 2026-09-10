Dispatcher_FairPvPScore::~Dispatcher_FairPvPScore() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0822793c orig/ours=300/292 ndiff-blocks=51
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movw   $0x0,-0x16(%ebp) | lea    -0x16(%ebp),%eax | movl   $&_ZZN23Dispatcher_FairPvPScore12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(
