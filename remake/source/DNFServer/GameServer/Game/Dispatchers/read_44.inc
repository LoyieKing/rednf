int Dispatcher_CodeCheckSum::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// process 依赖（清单外）：state!=3→7；count(msg+0x10)!=1→0x16；
// GetInvenSlot(EQUIP,slot) 后 find_item 缺失→0x11、持有数不足→0x16；
// 非 0x12ca 物品→0x17；0x12ca 时摆摊忙→0x3c、时光门开→0x16，否则 0。
// （声明需入 Dispatchers.h，见 NEEDS_DECL）
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e329e orig/ours=78/84 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x10(%ebp) | movzwl 0x10(%eax),%eax
// [b28d-solo-Dispatchers] P211 callee 侧：返回类型 int→unsigned int（头声明同步，
//   mangled 不变），使 process 调用点 store 翻为 ORIG 的 RHS-first+edx 中转。
