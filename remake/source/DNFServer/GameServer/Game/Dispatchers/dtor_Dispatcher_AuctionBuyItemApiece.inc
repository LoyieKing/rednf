Dispatcher_AuctionBuyItemApiece::~Dispatcher_AuctionBuyItemApiece() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08214e44 orig/ours=210/224 ndiff-blocks=35
//   帧差 sub $60 vs $70 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x60,%esp | lea    -0x56(%ebp),%eax | mov    %eax,-0x44(%ebp)
