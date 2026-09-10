Dispatcher_PowerWarProcessInfo::~Dispatcher_PowerWarProcessInfo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821ea32 orig/ours=83/88 ndiff-blocks=13
//   帧差 sub $20 vs $30 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x20,%esp | movl   $&_ZZN30Dispatcher_PowerWarProcessInfo12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,%ebx
