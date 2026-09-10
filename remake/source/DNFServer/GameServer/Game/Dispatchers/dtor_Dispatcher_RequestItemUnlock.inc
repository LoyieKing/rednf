Dispatcher_RequestItemUnlock::~Dispatcher_RequestItemUnlock() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x08222f82 orig/ours=144/144 ndiff-blocks=26
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: movb   $0x0,-0x11(%ebp) | movw   $0x0,-0x14(%ebp) | lea    -0x11(%ebp),%eax
