Dispatcher_GuildAttendanceInfo::~Dispatcher_GuildAttendanceInfo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d5492 orig/ours=59/59 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movzwl -0x1d(%ebp),%eax | movzwl %ax,%esi | mov    &_ZN10GlobalData17s_guild_proxy_mgrE,%edx
