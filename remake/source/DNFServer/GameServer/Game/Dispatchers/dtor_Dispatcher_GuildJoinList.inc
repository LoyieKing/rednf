Dispatcher_GuildJoinList::~Dispatcher_GuildJoinList() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d52a6 orig/ours=68/69 ndiff-blocks=7
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | movl   $0x0,-0xc(%ebp)
