Dispatcher_AuctionMyRegistedItemInfo::~Dispatcher_AuctionMyRegistedItemInfo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08216966 orig/ours=141/147 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x21(%ebp),%eax | movl   $&_ZZN36Dispatcher_AuctionMyRegistedItemInfo12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movzb
