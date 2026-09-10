Dispatcher_SkillInit::~Dispatcher_SkillInit() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e5a2c orig/ours=62/61 ndiff-blocks=4
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0xc(%ebp),%eax | cmp    $0xffffffff,%eax | mov    -0xc(%ebp),%eax
