Dispatcher_ComboSkillExtensionQuickSlotReset::~Dispatcher_ComboSkillExtensionQuickSlotReset() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825f594 orig/ours=41/40 ndiff-blocks=3
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: je     0825f602 <+0x6e> | jmp    0825f607 <+0x73> | mov    $0x0,%eax
