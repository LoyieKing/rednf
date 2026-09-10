Dispatcher_SetSubGuildMaster::~Dispatcher_SetSubGuildMaster() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082053e6 orig/ours=106/149 ndiff-blocks=20
//   帧差 sub $54 vs $6c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x54,%esp | movl   $&_ZZN28Dispatcher_SetSubGuildMaster12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp) | lea    -0x3d(%ebp),
