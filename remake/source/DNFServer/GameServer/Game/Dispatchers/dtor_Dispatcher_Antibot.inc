Dispatcher_Antibot::~Dispatcher_Antibot() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0820b50c orig/ours=191/200 ndiff-blocks=33
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: push   %edi | push   %esi | push   %ebx
