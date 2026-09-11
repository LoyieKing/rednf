// ==== Dispatcher_StackableActionUse::process (ORIG 0x0825e11e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0825e11e orig/ours=145/158 ndiff-blocks=25
//   帧差 sub $4 vs $90 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: add    $0xffffff80,%esp | mov    %eax,-0x14(%ebp) | mov    0x14(%ebp),%eax
int Dispatcher_StackableActionUse::process(CUser* user, MSG_BASE& msg, ParamBase& param)
