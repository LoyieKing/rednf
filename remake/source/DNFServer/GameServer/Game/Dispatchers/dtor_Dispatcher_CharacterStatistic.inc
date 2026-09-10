Dispatcher_CharacterStatistic::~Dispatcher_CharacterStatistic() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082051d0 orig/ours=124/128 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN29Dispatcher_CharacterStatistic12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x20(%ebp),%eax | add    $0x8,
