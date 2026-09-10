Dispatcher_Shop_Coin_Event::~Dispatcher_Shop_Coin_Event() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081ea622 orig/ours=199/205 ndiff-blocks=14
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: movl   $0x1,-0x14(%ebp) | mov    &_ZN10GlobalData15s_event_managerE,%eax | mov    %eax,-0x10(%ebp)
