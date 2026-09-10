Dispatcher_LevelupSupportReqestItem::~Dispatcher_LevelupSupportReqestItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0825fc3a orig/ours=169/178 ndiff-blocks=36
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x40,%esp | mov    $0xa49,%eax | mov    $0x0,%eax
