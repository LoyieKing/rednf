Dispatcher_Mercenary_Competition::~Dispatcher_Mercenary_Competition() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ddd00 orig/ours=89/93 ndiff-blocks=15
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    0x10(%ebp),%eax
