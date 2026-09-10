Dispatcher_GuildCargoPopItem::~Dispatcher_GuildCargoPopItem() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d15b2 orig/ours=119/126 ndiff-blocks=22
//   帧差 sub $5c vs $6c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x5c,%esp | mov    %eax,-0x24(%ebp) | mov    %eax,%edx
