Dispatcher_DieAssaultPlayer::~Dispatcher_DieAssaultPlayer() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08207208 orig/ours=146/145 ndiff-blocks=23
//   帧差 sub $34 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x34,%esp | movl   $&_ZZN27Dispatcher_DieAssaultPlayer12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
