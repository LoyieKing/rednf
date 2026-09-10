Dispatcher_SelectUltimateDifficulty::~Dispatcher_SelectUltimateDifficulty() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08226f30 orig/ours=166/160 ndiff-blocks=37
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    $0x0,%ebx | mov    %eax,-0xc(%ebp) | cmpl   $0x0,-0xc(%ebp)
