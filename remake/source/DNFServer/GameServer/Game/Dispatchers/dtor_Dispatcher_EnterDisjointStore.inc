Dispatcher_EnterDisjointStore::~Dispatcher_EnterDisjointStore() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d01ae orig/ours=64/61 ndiff-blocks=10
//   帧差 sub $24 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x24,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x18(%ebp)
