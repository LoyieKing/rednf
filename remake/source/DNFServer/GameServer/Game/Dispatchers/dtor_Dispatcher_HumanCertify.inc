Dispatcher_HumanCertify::~Dispatcher_HumanCertify() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082064d6 orig/ours=274/288 ndiff-blocks=35
//   帧差 sub $48 vs $68 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x48,%esp | movl   $&_ZZN23Dispatcher_HumanCertify12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | cmp    $0x251b,%eax
