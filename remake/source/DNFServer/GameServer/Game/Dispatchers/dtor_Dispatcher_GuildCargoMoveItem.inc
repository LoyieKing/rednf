Dispatcher_GuildCargoMoveItem::~Dispatcher_GuildCargoMoveItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d1954 orig/ours=93/94 ndiff-blocks=18
//   帧差 sub $40 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x40,%esp
