Dispatcher_ChangeTutorialFlag::~Dispatcher_ChangeTutorialFlag() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N96] ORIG 0x0820697c orig/ours=118/118 ndiff-blocks=15
//   P0 清绑：原 `register asm("reg")` 存量绑定已删（N96 全域禁绑，不得恢复）；
//   清绑后残余寄存器/槽序差按 N96+N18a 封顶（实测内联/unsigned 化等自然形不闭合）。
//   首差: sub    $0x38,%esp | movl   $&_ZZN29Dispatcher_ChangeTutorialFlag12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $0x0,-0x14(
