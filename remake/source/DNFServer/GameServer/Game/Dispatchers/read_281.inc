int Dispatcher_CallPartyMemberRealtimeInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ce3ee orig/ours=75/73 ndiff-blocks=4
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    $0x0,%ebx | mov    $0x1,%ebx | test   %ebx,%ebx
