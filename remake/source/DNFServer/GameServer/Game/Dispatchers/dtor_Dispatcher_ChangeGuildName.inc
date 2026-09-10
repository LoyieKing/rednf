Dispatcher_ChangeGuildName::~Dispatcher_ChangeGuildName() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082134a8 orig/ours=269/272 ndiff-blocks=13
//   帧差 sub $8c vs $90 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | sub    $0x8c,%esp | movl   $&_ZZN26Dispatcher_ChangeGuildName12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
