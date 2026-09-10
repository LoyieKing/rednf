Dispatcher_RequestPCRoomPlayerList::~Dispatcher_RequestPCRoomPlayerList() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0821d7fa orig/ours=56/64 ndiff-blocks=13
//   帧差 sub $20 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x20,%esp | jne    0821d828 <+0x2e>
