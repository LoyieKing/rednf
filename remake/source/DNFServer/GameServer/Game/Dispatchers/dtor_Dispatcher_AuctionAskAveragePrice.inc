Dispatcher_AuctionAskAveragePrice::~Dispatcher_AuctionAskAveragePrice() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08213940 orig/ours=295/333 ndiff-blocks=35
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x94,%esp | lea    -0x2d(%ebp),%eax | movl   $&_ZZN33Dispatcher_AuctionAskAveragePrice12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,
