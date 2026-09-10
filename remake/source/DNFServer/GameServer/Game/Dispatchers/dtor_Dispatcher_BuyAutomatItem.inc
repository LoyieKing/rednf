Dispatcher_BuyAutomatItem::~Dispatcher_BuyAutomatItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081fe578 orig/ours=333/340 ndiff-blocks=63
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN25Dispatcher_BuyAutomatItem12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x28(%ebp),%eax | movl   $"pUser d
