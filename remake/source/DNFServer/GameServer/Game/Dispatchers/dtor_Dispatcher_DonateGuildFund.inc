Dispatcher_DonateGuildFund::~Dispatcher_DonateGuildFund() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d4970 orig/ours=127/120 ndiff-blocks=20
//   帧差 sub $3c vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | sub    $0x3c,%esp
