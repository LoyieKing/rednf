Dispatcher_ItemHyperLinkMessage::~Dispatcher_ItemHyperLinkMessage() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081da514 orig/ours=40/35 ndiff-blocks=8
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: jne    081da527 <+0x13> | mov    $0xffffffff,%eax | jmp    081da581 <+0x6d>
