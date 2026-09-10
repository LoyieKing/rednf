Dispatcher_ReportBadP2PUser::~Dispatcher_ReportBadP2PUser() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081fd0a8 orig/ours=106/104 ndiff-blocks=16
//   帧差 sub $40 vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x40,%esp | lea    -0x32(%ebp),%eax
