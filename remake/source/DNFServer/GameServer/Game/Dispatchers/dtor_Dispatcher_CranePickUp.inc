Dispatcher_CranePickUp::~Dispatcher_CranePickUp() {}
// 【batch-17 X1 池收口（§6.2）】orig 185 / ours 192（gap -7）：调用点差集
//   33↔33 **归零**（残差全是 PacketGuard/cMyTrace 的 C2/D2 ↔ C1/D1 同体
//   符号名对称差）；多出的 7 insn 为 4.4 的 EH cleanup 与守卫落槽
//   （N83/N82 族）⇒ 语义完成、byte 层封顶。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0821d508 orig/ours=185/192 ndiff-blocks=29
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    $0x0,%eax | xor    $0x1,%eax | mov    $0x0,%eax
