Dispatcher_CheckJoinGuild::~Dispatcher_CheckJoinGuild() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d4bf6 orig/ours=63/63 ndiff-blocks=9
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: sub    $0x38,%esp | mov    0x10(%ebp),%eax | mov    %eax,-0xc(%ebp)
