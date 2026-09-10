Dispatcher_Client_Spec_Statistic::~Dispatcher_Client_Spec_Statistic() {}
// ORIG 0x081ce794（报告 Dispatcher_Client_Spec_Statistic/send.md）[随行依赖]：
// 构造 Packet_Client_Spec_Statistic（ORIG W 0x0822bc18），写 flag/count/len，
// memcpy hwSpec 条目后经统计代理 SendPacket。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081ce794 orig/ours=86/117 ndiff-blocks=15
//   帧差 sub $c20 vs $c3c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0xc20,%esp | mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp)
