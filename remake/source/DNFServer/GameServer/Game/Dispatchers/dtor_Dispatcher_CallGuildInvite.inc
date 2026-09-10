Dispatcher_CallGuildInvite::~Dispatcher_CallGuildInvite() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0820791c orig/ours=148/146 ndiff-blocks=16
//   帧差 sub $40 vs $58 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x40,%esp
