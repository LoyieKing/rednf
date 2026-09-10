Dispatcher_ConnectP2PAssault::~Dispatcher_ConnectP2PAssault() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08206fea orig/ours=124/128 ndiff-blocks=22
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | movl   $&_ZZN28Dispatcher_ConnectP2PAssault12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
