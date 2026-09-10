Dispatcher_Mercenary_Return::~Dispatcher_Mercenary_Return() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081dd99c orig/ours=42/52 ndiff-blocks=11
//   帧差 sub $28 vs $24 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x28,%esp | mov    0x10(%ebp),%eax | mov    0x14(%ebp),%eax
