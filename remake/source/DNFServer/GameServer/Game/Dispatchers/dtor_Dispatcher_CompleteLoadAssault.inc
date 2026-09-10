Dispatcher_CompleteLoadAssault::~Dispatcher_CompleteLoadAssault() {}
// ORIG 0x08206f10（报告 Dispatcher_CompleteLoadAssault/dispatch_sig.md）：
// state<=1 → LineFunc(0xa199)；room+0x6e4 matching 非空 → IMatch 虚表 +0x54
// （OnCompleteLoad）vcall；否则 GetInstanceAssaultMgr()->OnCompleteLoad(user)
// 返回值 != 1 → LineFunc(0xa1a8)；恒 return 0。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08206f10 orig/ours=57/59 ndiff-blocks=13
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: movl   $&_ZZN30Dispatcher_CompleteLoadAssault12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,-0xc(%ebp) | cmpl   $0x0,
