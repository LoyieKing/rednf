Dispatcher_HeroMissionEventState_Reward::~Dispatcher_HeroMissionEventState_Reward() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08260094 orig/ours=128/132 ndiff-blocks=24
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $0x0,-0x10(%ebp) | lea    -0x10(%ebp),%eax | movl   $&_ZZN39Dispatcher_HeroMissionEventState_Reward12dispatch_sigEP5CUserR9PacketBufE19__PRETTY
