Dispatcher_GuildMasterDelegate::~Dispatcher_GuildMasterDelegate() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082082d6 orig/ours=129/128 ndiff-blocks=18
//   帧差 sub $50 vs $68 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x50,%esp
