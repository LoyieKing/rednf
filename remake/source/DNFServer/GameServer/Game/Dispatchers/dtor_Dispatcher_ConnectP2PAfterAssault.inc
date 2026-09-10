Dispatcher_ConnectP2PAfterAssault::~Dispatcher_ConnectP2PAfterAssault() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08206cfa orig/ours=122/124 ndiff-blocks=21
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | movl   $&_ZZN33Dispatcher_ConnectP2PAfterAssault12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
