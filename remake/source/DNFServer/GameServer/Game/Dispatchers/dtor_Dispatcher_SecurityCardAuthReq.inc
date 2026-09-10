Dispatcher_SecurityCardAuthReq::~Dispatcher_SecurityCardAuthReq() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0820a2c4 orig/ours=90/96 ndiff-blocks=7
//   帧差 sub $18 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x18,%esp | movl   $&_ZZN30Dispatcher_SecurityCardAuthReq12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | seta   %al
