Dispatcher_ApproveJoinGuild::~Dispatcher_ApproveJoinGuild() {}
// ORIG 0x081d50b4 T _ZN27Dispatcher_ApproveJoinGuild7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_ApproveJoinGuild/process.md 汇编+反编译 C 全文转写）
// local_26 = Packet_Approve_Join_Guild（ORIG 0x16 字节，shared/packet 权威布局）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d50b4 orig/ours=70/73 ndiff-blocks=13
//   帧差 sub $30 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x30,%esp | mov    %eax,-0xc(%ebp)
