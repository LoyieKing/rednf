Inter_AuctionNotifyAuctionService::~Inter_AuctionNotifyAuctionService() {}
// ORIG 0x084d7ac8 T dispatch_sig —— 拍卖服务开关：置 proxy 运行态并全员广播。
// 报告 docs/class_func_reports/Inter_AuctionNotifyAuctionService/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=90（ORIG 0x084d7ac8）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=95（ORIG 0x84d7ac8）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
