Dispatcher_Delete_GuildBoard_Text::~Dispatcher_Delete_GuildBoard_Text() {}
// ORIG 0x081d33a0（报告 Dispatcher_Delete_GuildBoard_Text/process.md）：
// param+0x89(unsigned)=待删公告文本索引；无状态门槛。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d33a0 orig/ours=39/35 ndiff-blocks=6
//   帧差 sub $2c vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
