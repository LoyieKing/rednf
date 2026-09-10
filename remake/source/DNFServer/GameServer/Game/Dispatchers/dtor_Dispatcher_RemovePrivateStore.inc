Dispatcher_RemovePrivateStore::~Dispatcher_RemovePrivateStore() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08202bb6 orig/ours=83/84 ndiff-blocks=16
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x20,%esp | jne    08202be4 <+0x2e> | lea    -0x18(%ebp),%eax
