Dispatcher_SaveGameOption_QuickChatting::~Dispatcher_SaveGameOption_QuickChatting() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cf1ac orig/ours=118/121 ndiff-blocks=23
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x18(%ebp) | mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp)
