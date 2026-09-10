Dispatcher_Back2Village::~Dispatcher_Back2Village() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0820559a orig/ours=137/139 ndiff-blocks=9
//   帧差 sub $30 vs $20 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    %eax,-0x10(%ebp) | cmpl   $0x0,-0x10(%ebp)
