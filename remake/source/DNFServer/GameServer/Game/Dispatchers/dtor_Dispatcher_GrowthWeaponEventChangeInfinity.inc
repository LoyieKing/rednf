Dispatcher_GrowthWeaponEventChangeInfinity::~Dispatcher_GrowthWeaponEventChangeInfinity() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e7f2e orig/ours=117/122 ndiff-blocks=17
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | mov    -0x10(%ebp),%eax
