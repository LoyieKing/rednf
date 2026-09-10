Dispatcher_CharacSlotExtendEffect::~Dispatcher_CharacSlotExtendEffect() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d7fb6 orig/ours=150/151 ndiff-blocks=21
//   帧差 sub $4c vs $50 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x4c,%esp | mov    $0x0,%ebx
