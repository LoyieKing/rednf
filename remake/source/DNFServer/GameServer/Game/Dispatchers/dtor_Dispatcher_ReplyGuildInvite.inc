Dispatcher_ReplyGuildInvite::~Dispatcher_ReplyGuildInvite() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08207b52 orig/ours=126/131 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $0x0,0xc(%esp) | movl   $0x0,0x8(%esp) | movl   $&_ZZN27Dispatcher_ReplyGuildInvite12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(
