// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081dda1c orig/ours=86/87 ndiff-blocks=19
//   帧差 sub $20 vs $30 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x20,%esp | lea    -0x18(%ebp),%eax | lea    -0x18(%ebp),%eax
void Dispatcher_Mercenary_Return::send(CUser* user, ParamBase& param)     // ORIG 0x081dda1c (86 insn)
