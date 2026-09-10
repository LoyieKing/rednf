Dispatcher_ConnectP2PPvP::~Dispatcher_ConnectP2PPvP() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08225394 orig/ours=134/141 ndiff-blocks=14
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: movl   $&_ZZN24Dispatcher_ConnectP2PPvP12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN24Dispatcher_ConnectP2PPvP12di
