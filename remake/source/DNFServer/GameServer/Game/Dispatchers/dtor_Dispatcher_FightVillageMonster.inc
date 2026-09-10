Dispatcher_FightVillageMonster::~Dispatcher_FightVillageMonster() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082226c0 orig/ours=101/99 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN30Dispatcher_FightVillageMonster12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movw   $0x0,-0x12(%ebp) | lea    -0x1
