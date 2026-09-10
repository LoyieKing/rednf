Dispatcher_WithdrawMoney::~Dispatcher_WithdrawMoney() {}
// [b28c-31] N82：ORIG 0x082251xx 复用 item 对象死亡后的栈槽（GetMoney 结果落
// -0x46），4.4 不复用槽且 money/-0x10 形态不同 ⇒ 槽复用策略跨版本（N82）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x082250ec orig/ours=73/71 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movl   $&_ZZN24Dispatcher_WithdrawMoney12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,-0x46(%ebp) | mov    -0x10(%ebp
