int Dispatcher_ScanBotByDrv::check_error(CUser* user, MSG_BASE& msg) { return 0; }
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08264740 orig/ours=57/59 ndiff-blocks=14
//   帧差 sub $34 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x34,%esp | mov    %eax,-0x14(%ebp)
