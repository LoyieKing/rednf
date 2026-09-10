Dispatcher_AuctionBidding::~Dispatcher_AuctionBidding() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0821522e orig/ours=462/484 ndiff-blocks=69
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x31(%ebp),%eax | movl   $&_ZZN25Dispatcher_AuctionBidding12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movzbl -0x31(%eb
