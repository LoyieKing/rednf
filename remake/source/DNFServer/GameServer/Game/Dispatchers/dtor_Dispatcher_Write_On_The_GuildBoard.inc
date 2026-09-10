Dispatcher_Write_On_The_GuildBoard::~Dispatcher_Write_On_The_GuildBoard() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d31ee orig/ours=93/98 ndiff-blocks=18
//   帧差 sub $4c vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | sub    $0x4c,%esp
