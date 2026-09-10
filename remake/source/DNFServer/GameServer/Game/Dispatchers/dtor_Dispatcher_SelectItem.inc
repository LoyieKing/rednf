Dispatcher_SelectItem::~Dispatcher_SelectItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0822997a orig/ours=470/457 ndiff-blocks=42
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x1cc,%esp | movb   $0x0,-0x4a(%ebp) | movl   $&_ZZN21Dispatcher_SelectItem12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
