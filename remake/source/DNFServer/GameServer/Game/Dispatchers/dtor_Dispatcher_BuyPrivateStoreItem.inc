Dispatcher_BuyPrivateStoreItem::~Dispatcher_BuyPrivateStoreItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x08202322 orig/ours=160/162 ndiff-blocks=23
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x24,%esp | jne    0820234f <+0x2d> | movl   $0x0,0xc(%esp)
