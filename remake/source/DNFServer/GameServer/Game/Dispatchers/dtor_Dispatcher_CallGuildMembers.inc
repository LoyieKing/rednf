Dispatcher_CallGuildMembers::~Dispatcher_CallGuildMembers() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ffa56 orig/ours=58/66 ndiff-blocks=11
//   帧差 sub $10 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x10,%esp
