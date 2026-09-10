Dispatcher_CallGuildInfo::~Dispatcher_CallGuildInfo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08200448 orig/ours=161/168 ndiff-blocks=16
//   帧差 sub $1bc vs $1c0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x1bc,%esp | movl   $&_ZZN24Dispatcher_CallGuildInfo12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
