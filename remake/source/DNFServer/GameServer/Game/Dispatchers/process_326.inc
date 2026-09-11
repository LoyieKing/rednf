// ==== Dispatcher_FatigueAttendance::process (ORIG 0x081daffa) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081daffa orig/ours=215/217 ndiff-blocks=29
//   帧差 sub $40 vs $54 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x40,%esp | mov    %eax,-0x2c(%ebp)
int Dispatcher_FatigueAttendance::process(CUser* user, MSG_BASE& msg, ParamBase& param)
