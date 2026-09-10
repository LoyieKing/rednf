Dispatcher_Overflow_Info::~Dispatcher_Overflow_Info() {}
// ORIG 0x0821e4b6（报告 Dispatcher_Overflow_Info/dispatch_sig.md）：
// Packet_Overflow_Statistic_Add(0xc43,0xd) = PacketHeader(10B)+0xa uchar+0xb ushort。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821e4b6 orig/ours=55/59 ndiff-blocks=11
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | lea    -0x15(%ebp),%eax
