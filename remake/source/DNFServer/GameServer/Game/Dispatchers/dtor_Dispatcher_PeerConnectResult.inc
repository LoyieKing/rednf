Dispatcher_PeerConnectResult::~Dispatcher_PeerConnectResult() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x082033e6 orig/ours=77/76 ndiff-blocks=11
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: sub    $0x24,%esp | movb   $0x0,-0x15(%ebp) | movw   $0x0,-0x18(%ebp)
