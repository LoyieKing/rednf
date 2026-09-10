Dispatcher_EnterPrivateStore::~Dispatcher_EnterPrivateStore() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x082025ae orig/ours=63/72 ndiff-blocks=15
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: push   %ebx | sub    $0x24,%esp | jne    082025d4 <+0x26>
