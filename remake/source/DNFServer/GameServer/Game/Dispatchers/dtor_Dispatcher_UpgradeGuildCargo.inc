Dispatcher_UpgradeGuildCargo::~Dispatcher_UpgradeGuildCargo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08222958 orig/ours=303/331 ndiff-blocks=47
//   帧差 sub $ac vs $d0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0xac,%esp | cmp    $0x3,%eax
