Dispatcher_Antibot_DP::~Dispatcher_Antibot_DP() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0820b322 orig/ours=122/116 ndiff-blocks=23
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: push   %edi | push   %esi | sub    $0xb5c,%esp
