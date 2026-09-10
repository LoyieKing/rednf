Dispatcher_BiddingRoutingItem::~Dispatcher_BiddingRoutingItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082255ca orig/ours=116/114 ndiff-blocks=9
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: cmp    $0x5,%eax | mov    $0x0,%eax | nop
