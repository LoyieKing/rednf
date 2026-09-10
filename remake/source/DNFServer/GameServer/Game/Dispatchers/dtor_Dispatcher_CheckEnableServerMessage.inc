Dispatcher_CheckEnableServerMessage::~Dispatcher_CheckEnableServerMessage() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821df6a orig/ours=164/168 ndiff-blocks=29
//   帧差 sub $40 vs $50 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x40,%esp | mov    %eax,%ebx | mov    %eax,%esi
