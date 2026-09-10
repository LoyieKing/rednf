Dispatcher_CancelJoinGuild::~Dispatcher_CancelJoinGuild() {}
// ORIG 0x081d4fcc（报告 Dispatcher_CancelJoinGuild/process.md）：
// state==3 且 guildkey==0；param+0xd(unsigned)=申请索引；
// 成功路径末尾 SendCmdOkPacket(0x15e)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d4fcc orig/ours=48/42 ndiff-blocks=6
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | push   %ebx | sub    $0x20,%esp
