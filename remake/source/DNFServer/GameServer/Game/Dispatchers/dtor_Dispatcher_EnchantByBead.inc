Dispatcher_EnchantByBead::~Dispatcher_EnchantByBead() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d24b8 orig/ours=75/69 ndiff-blocks=9
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp) | mov    %eax,%edx
