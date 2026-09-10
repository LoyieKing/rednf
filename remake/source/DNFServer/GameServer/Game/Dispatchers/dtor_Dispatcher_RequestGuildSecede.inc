Dispatcher_RequestGuildSecede::~Dispatcher_RequestGuildSecede() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x08207d3a orig/ours=177/181 ndiff-blocks=23
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: push   %esi | push   %ebx | sub    $0x50,%esp
