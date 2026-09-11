// ==== Dispatcher_PartyTeleport::process (ORIG 0x081dbfd6) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081dbfd6 orig/ours=129/138 ndiff-blocks=13
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    -0x2c(%ebp),%edx | mov    %eax,0x4(%edx) | mov    0x10(%ebp),%eax
int Dispatcher_PartyTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
