Dispatcher_RefreshGuildInfo::~Dispatcher_RefreshGuildInfo() {}
// ORIG 0x081d4886（报告 Dispatcher_RefreshGuildInfo/process.md）：
// state>2 且 guildkey!=0；characNo 经 get_charac_no(-1)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d4886 orig/ours=49/45 ndiff-blocks=9
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | push   %ebx | sub    $0x10,%esp
