Inter_AuctionResultItemList::~Inter_AuctionResultItemList() {}
// ORIG 0x084d75bc T dispatch_sig —— 拍卖物品列表应答：GM 延迟追踪 + 0xbd 包转发余包。
// 报告 docs/class_func_reports/Inter_AuctionResultItemList/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=69（ORIG 0x084d75bc）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=69（ORIG 0x84d75bc）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
