Dispatcher_CodeCheckSum::~Dispatcher_CodeCheckSum() {}
// [b28c-31] P0 清绑（原 register char* pp asm("eax") + register bool notBak
// asm("al") 已删）；残差 N18a：ORIG 0x0820459a 逐参立即 `lea path→%eax` 后压栈，
// 4.4 提前物化第二 lea 到 %edx（`lea -0x34,%edx`）⇒ 单段装载序不可造。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082044b0 orig/ours=101/99 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN23Dispatcher_CodeCheckSum12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $&_ZZN23Dispatcher_CodeCheckSum12disp
