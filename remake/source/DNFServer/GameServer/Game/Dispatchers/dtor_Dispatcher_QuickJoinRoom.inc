Dispatcher_QuickJoinRoom::~Dispatcher_QuickJoinRoom() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0820350e orig/ours=182/191 ndiff-blocks=12
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x90,%esp | movl   $&_ZZN24Dispatcher_QuickJoinRoom12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x79(%ebp),%eax
