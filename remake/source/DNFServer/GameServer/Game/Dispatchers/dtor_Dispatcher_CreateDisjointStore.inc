Dispatcher_CreateDisjointStore::~Dispatcher_CreateDisjointStore() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081cf986 orig/ours=99/93 ndiff-blocks=18
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: sub    $0x34,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x18(%ebp)
