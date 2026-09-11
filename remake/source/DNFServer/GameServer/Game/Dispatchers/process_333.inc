// ==== Dispatcher_TournamentRewardSelect::process (ORIG 0x081e00f0) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e00f0 orig/ours=126/122 ndiff-blocks=17
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x1c(%ebp)
int Dispatcher_TournamentRewardSelect::process(CUser* user, MSG_BASE& msg, ParamBase& param)
