int Dispatcher_AbnormalUseStackable::check_error(CUser* user, MSG_BASE& msg) { return 0; }
// ORIG 0x082649ae（报告 Dispatcher_AbnormalUseStackable/process.md）：
// check_error==0 → addServerHackCnt(user, 0xfa0, 1, msg+0x10, msg+0x14)。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082649ae orig/ours=41/35 ndiff-blocks=6
//   帧差 sub $30 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x30,%esp
