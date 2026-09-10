Dispatcher_UpdateServerMessage::~Dispatcher_UpdateServerMessage() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821d9ee orig/ours=345/329 ndiff-blocks=61
//   帧差 sub $b0 vs $d0 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0xb0,%esp | mov    %eax,%ebx
