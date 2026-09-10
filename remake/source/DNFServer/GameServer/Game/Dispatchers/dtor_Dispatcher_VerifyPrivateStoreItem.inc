Dispatcher_VerifyPrivateStoreItem::~Dispatcher_VerifyPrivateStoreItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08229768 orig/ours=130/132 ndiff-blocks=18
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x24,%esp | movl   $&_ZZN33Dispatcher_VerifyPrivateStoreItem12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    &_ZN10G
