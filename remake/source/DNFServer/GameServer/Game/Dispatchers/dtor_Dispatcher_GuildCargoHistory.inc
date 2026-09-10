Dispatcher_GuildCargoHistory::~Dispatcher_GuildCargoHistory() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d0cb4 orig/ours=78/78 ndiff-blocks=16
//   帧差 sub $30 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x30,%esp | mov    0x14(%ebp),%eax
