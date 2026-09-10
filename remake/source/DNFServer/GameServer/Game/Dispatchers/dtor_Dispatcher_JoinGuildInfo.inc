Dispatcher_JoinGuildInfo::~Dispatcher_JoinGuildInfo() {}
// ORIG 0x081d5398（报告 Dispatcher_JoinGuildInfo/process.md）：
// state==3 且 guildkey==0；非综合 PvP 基础渠道时 serverGroup 取
// G_CEnvironment()->m_serverEnvir.m_serverGroup（env+0x378），否则 GetServerGroup()。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d5398 orig/ours=53/50 ndiff-blocks=8
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %ebx | sub    $0x24,%esp | setne  %al
