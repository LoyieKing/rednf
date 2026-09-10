Dispatcher_AuctionSearchByNoItmeKey::~Dispatcher_AuctionSearchByNoItmeKey() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082161e4 orig/ours=425/431 ndiff-blocks=57
//   帧差 sub $94 vs $a4 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x94,%esp | lea    -0x21(%ebp),%eax | movl   $&_ZZN35Dispatcher_AuctionSearchByNoItmeKey12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION_
