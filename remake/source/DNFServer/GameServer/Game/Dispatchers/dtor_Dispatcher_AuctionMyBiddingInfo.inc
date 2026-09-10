Dispatcher_AuctionMyBiddingInfo::~Dispatcher_AuctionMyBiddingInfo() {}
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08216b9a orig/ours=94/98 ndiff-blocks=14
//   帧差 sub $44 vs $54 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x44,%esp | mov    &_ZN10GlobalData15s_auction_proxyE,%eax | movl   $&_ZZN31Dispatcher_AuctionMyBiddingInfo12dispatch_sigEP5CUserR9PacketBufE1
