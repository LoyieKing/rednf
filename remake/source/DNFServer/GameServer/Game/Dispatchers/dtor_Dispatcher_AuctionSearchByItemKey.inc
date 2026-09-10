Dispatcher_AuctionSearchByItemKey::~Dispatcher_AuctionSearchByItemKey() {}
// ============================================================================
// ORIG 0x082159fc（docs/class_func_reports/Dispatcher_AuctionSearchByItemKey/dispatch_sig.md）
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082159fc orig/ours=441/444 ndiff-blocks=65
//   帧差 sub $144 vs $154 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x144,%esp | lea    -0x29(%ebp),%eax | movl   $&_ZZN33Dispatcher_AuctionSearchByItemKey12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__
