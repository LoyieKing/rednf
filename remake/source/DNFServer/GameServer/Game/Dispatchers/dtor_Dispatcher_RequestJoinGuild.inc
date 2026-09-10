Dispatcher_RequestJoinGuild::~Dispatcher_RequestJoinGuild() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d4e28 orig/ours=89/98 ndiff-blocks=7
//   帧差 sub $68 vs $64 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x68,%esp | mov    0x10(%ebp),%eax | mov    %eax,-0xc(%ebp)
