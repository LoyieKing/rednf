Dispatcher_SecurityCardRetransfer::~Dispatcher_SecurityCardRetransfer() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0820a9a0 orig/ours=277/228 ndiff-blocks=30
//   帧差 sub $18c vs $180 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x18c,%esp | movl   $&_ZZN33Dispatcher_SecurityCardRetransfer12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
