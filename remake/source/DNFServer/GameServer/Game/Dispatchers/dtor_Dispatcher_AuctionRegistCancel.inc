Dispatcher_AuctionRegistCancel::~Dispatcher_AuctionRegistCancel() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08214b96 orig/ours=165/176 ndiff-blocks=27
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x29(%ebp),%eax | movl   $&_ZZN30Dispatcher_AuctionRegistCancel12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movzbl -0x2
