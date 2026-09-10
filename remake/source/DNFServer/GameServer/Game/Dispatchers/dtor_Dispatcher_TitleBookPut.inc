Dispatcher_TitleBookPut::~Dispatcher_TitleBookPut() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d774e orig/ours=154/154 ndiff-blocks=7
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: je     081d788e <+0x140> | movl   $0x0,0x4(%esp) | lea    -0x1c(%ebp),%eax
