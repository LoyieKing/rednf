Dispatcher_RequestOnTimeEventReward::~Dispatcher_RequestOnTimeEventReward() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0822458c orig/ours=212/207 ndiff-blocks=29
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: push   %edi | push   %esi | push   %ebx
