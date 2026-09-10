Dispatcher_PvPChannelInfo::~Dispatcher_PvPChannelInfo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082178c0 orig/ours=133/150 ndiff-blocks=20
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: setle  %al | setne  %al | test   %al,%al
