Dispatcher_GrowthWeaponEventUseMaterial::~Dispatcher_GrowthWeaponEventUseMaterial() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081e82fa orig/ours=119/128 ndiff-blocks=16
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x14(%ebp) | mov    -0x14(%ebp),%eax | mov    0x11(%eax),%ebx
