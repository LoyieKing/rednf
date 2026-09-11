// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08204710 orig/ours=225/248 ndiff-blocks=38
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: lea    -0x157(%ebp),%eax | movl   $&_ZZN30Dispatcher_CreatureSendMessage12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x
int Dispatcher_CreatureSendMessage::dispatch_sig(CUser* user, PacketBuf& packet)
