Dispatcher_DisguiseRequest::~Dispatcher_DisguiseRequest() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x0821b256 orig/ours=149/161 ndiff-blocks=24
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: sub    $0x20,%esp | jne    0821b26e <+0x18> | mov    $0x0,%ebx
