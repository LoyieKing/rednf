Dispatcher_AuctionMyAuctionHistory::~Dispatcher_AuctionMyAuctionHistory() {}
// ORIG 0x08216d02（报告 Dispatcher_AuctionMyAuctionHistory/dispatch_sig.md）：
// IsRunning!=1 → 0；state<3 || curCharacR==0 → LineFunc(0xc864)；否则
// PCK 包 +0x0e/+0x12/+0x16 三 dword（GetUID/getCurCharacNo/get_acc_id）→ SendPacket。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08216d02 orig/ours=56/64 ndiff-blocks=8
//   帧差 sub $38 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    &_ZN10GlobalData15s_auction_proxyE,%eax | movl   $&_ZZN34Dispatcher_AuctionMyAuctionHistory12dispatch_sigEP5CUserR9PacketBu
