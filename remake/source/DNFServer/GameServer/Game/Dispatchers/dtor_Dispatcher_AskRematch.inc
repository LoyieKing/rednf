Dispatcher_AskRematch::~Dispatcher_AskRematch() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08227862 orig/ours=62/74 ndiff-blocks=11
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: jle    08227887 <+0x25> | jne    0822788e <+0x2c> | mov    $0x1,%eax
