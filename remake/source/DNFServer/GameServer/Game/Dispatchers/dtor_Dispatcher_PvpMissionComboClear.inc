Dispatcher_PvpMissionComboClear::~Dispatcher_PvpMissionComboClear() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d5a5e orig/ours=90/80 ndiff-blocks=9
//   帧差 sub $130 vs $138 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %ebx | sub    $0x130,%esp
