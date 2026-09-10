Dispatcher_Select_Item_Grwoth_Power::~Dispatcher_Select_Item_Grwoth_Power() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081db580 orig/ours=48/43 ndiff-blocks=5
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | push   %ebx | sub    $0x10,%esp
