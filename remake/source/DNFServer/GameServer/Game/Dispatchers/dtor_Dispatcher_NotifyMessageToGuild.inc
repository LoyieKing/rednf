Dispatcher_NotifyMessageToGuild::~Dispatcher_NotifyMessageToGuild() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08207ffe orig/ours=181/183 ndiff-blocks=25
//   帧差 sub $90 vs $a0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x90,%esp | movl   $&_ZZN31Dispatcher_NotifyMessageToGuild12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | mov    %eax,-0xc
